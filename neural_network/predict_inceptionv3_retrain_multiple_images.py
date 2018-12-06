
import argparse  
import sys  
import tensorflow as tf
import numpy as np
import re
import os
import time

import csv
import hashlib

from tensorflow.python.framework import graph_util  
from tensorflow.python.framework import tensor_shape  
from tensorflow.python.platform import gfile  
from tensorflow.python.util import compat  

# Parallel stuff 
import time
from joblib import Parallel, delayed
import requests
import multiprocessing


LABE_LOOKUP_PATH = './inception_model/imagenet_2012_challenge_label_map_proto.pbtxt'  
UID_LOOKUP_PATH  = './inception_model/imagenet_synset_to_human_label_map.txt'
PATH_TO_PB_GRAPH = './inception_model/classify_image_graph_def.pb'
PATH_TO_TEST_IMAGES = '/Users/hossam.amer/7aS7aS_Works/work/jpeg_ml_research/inceptionv3/inceptionv3_flowers_QF_regularization/flower_photos/roses/' 
PATH_TO_BOTTLENECK_GRAPH = '/Users/hossam.amer/7aS7aS_Works/work/jpeg_ml_research/inceptionv3/classify_image_graph_def.pb'

# Model constants
BOTTLENECK_TENSOR_NAME = 'pool_3/_reshape:0'  
MODEL_INPUT_WIDTH = 299  
MODEL_INPUT_HEIGHT = 299  
MODEL_INPUT_DEPTH = 3  
JPEG_DATA_TENSOR_NAME = 'DecodeJpeg/contents:0'  
RESIZED_INPUT_TENSOR_NAME = 'ResizeBilinear:0'  
MAX_NUM_IMAGES_PER_CLASS = 2 ** 27 - 1  # ~134M  
START_QF = 10
END_QF = 100 + 10
QF_STEP_SIZE = 10
NUMBER_OF_QUALITY_FACTORS = int((END_QF - START_QF) / (QF_STEP_SIZE))
BOTTLENECK_TENSOR_SIZE = 2048*(1 + NUMBER_OF_QUALITY_FACTORS)
SIZE_OF_ONE_BOTTLENECK = int(BOTTLENECK_TENSOR_SIZE/(1 + NUMBER_OF_QUALITY_FACTORS))  


# Change class ID to human readable label
class NodeLookup(object):
    def __init__(self):  
        label_lookup_path = LABE_LOOKUP_PATH
        uid_lookup_path  = UID_LOOKUP_PATH
        self.node_lookup = self.load(label_lookup_path, uid_lookup_path)

    def load(self, label_lookup_path, uid_lookup_path):
        # 加载分类字符串n********对应分类名称的文件
        proto_as_ascii_lines = tf.gfile.GFile(uid_lookup_path).readlines()
        uid_to_human = {}
        #一行一行读取数据
        for line in proto_as_ascii_lines :
            #去掉换行符
            line=line.strip('\n')
            #按照'\t'分割
            parsed_items = line.split('\t')
            #获取分类编号
            uid = parsed_items[0]
            #获取分类名称
            human_string = parsed_items[1]
            #保存编号字符串n********与分类名称映射关系
            uid_to_human[uid] = human_string

        # 加载分类字符串n********对应分类编号1-1000的文件
        proto_as_ascii = tf.gfile.GFile(label_lookup_path).readlines()
        node_id_to_uid = {}
        for line in proto_as_ascii:
            if line.startswith('  target_class:'):
                #获取分类编号1-1000
                target_class = int(line.split(': ')[1])
            if line.startswith('  target_class_string:'):
                #获取编号字符串n********
                target_class_string = line.split(': ')[1]
                #保存分类编号1-1000与编号字符串n********映射关系
                node_id_to_uid[target_class] = target_class_string[1:-2]

        #建立分类编号1-1000对应分类名称的映射关系
        node_id_to_name = {}
        for key, val in node_id_to_uid.items():
            #获取分类名称
            name = uid_to_human[val]
            #建立分类编号1-1000到分类名称的映射关系
            node_id_to_name[key] = name
        return node_id_to_name

    #传入分类编号1-1000返回分类名称
    def id_to_string(self, node_id):
        if node_id not in self.node_lookup:
            return ''
        return self.node_lookup[node_id]

""""Utilized saved GraphDef to create a graph
  Returns： 
    Inception Graph with in var, para and tensors 
"""    
def create_inception_graph():  
  
  with tf.Session() as sess:  
    model_filename = PATH_TO_BOTTLENECK_GRAPH
    with gfile.FastGFile(model_filename, 'rb') as f:  
      graph_def = tf.GraphDef()  
      graph_def.ParseFromString(f.read())  
      bottleneck_tensor, jpeg_data_tensor, resized_input_tensor = (  
          tf.import_graph_def(graph_def, name='', return_elements=[  
              BOTTLENECK_TENSOR_NAME, JPEG_DATA_TENSOR_NAME,  
              RESIZED_INPUT_TENSOR_NAME]))  
  return sess.graph, bottleneck_tensor, jpeg_data_tensor, resized_input_tensor

#读取训练好的Inception-v3模型来创建graph
def create_graph():
  # the class that's been created from the textual definition in graph.proto
	with tf.gfile.FastGFile(PATH_TO_PB_GRAPH, 'rb') as f:  
		graph_def = tf.GraphDef()
		graph_def.ParseFromString(f.read())
		tf.import_graph_def(graph_def, name='')

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' # Hide the warning information from Tensorflow - annoying...


# Create a test image list with the corresponding image_dir
def create_test_image_lists(image_dir):
    if not gfile.Exists(image_dir):
    	print("Image directory '" + image_dir + "' not found.")  
    	return None

    result = {}  
    sub_dirs = [x[0] for x in gfile.Walk(image_dir)]
    is_root_dir = True  

    for sub_dir in sub_dirs:
      if is_root_dir:
      	is_root_dir = False
      	continue
      extensions = ['jpg', 'jpeg', 'JPG', 'JPEG']
      file_list = []

	  # class names
      dir_name = os.path.basename(sub_dir)

      if dir_name == image_dir:
      	continue
      

      print("Looking for images in '" + dir_name + "'")
      for extension in extensions:
      	file_glob = os.path.join(image_dir, dir_name, '*.' + extension)
      	file_list.extend(gfile.Glob(file_glob))

      	if not file_list:
      		print('No files found')
      		continue
      	if len(file_list) < 20:
      		print('WARNING: Folder has less than 20 images, which may cause issues.')
      	elif len(file_list) > MAX_NUM_IMAGES_PER_CLASS:
      		print('WARNING: Folder {} has more than {} images. Some images will '  
	    		'never be selected.'.format(dir_name, MAX_NUM_IMAGES_PER_CLASS))

      	label_name = re.sub(r'[^a-z0-9]+', ' ', dir_name.lower())
      	training_images = []
      	testing_images = []
      	validation_images = []
      	for file_name in file_list:
      		base_name = os.path.basename(file_name)
      		# We want to ignore anything after '_nohash_' in the file name when
	       # deciding which set to put an image in, the data set creator has a way of
	       # grouping photos that are close variations of each other. For example
	       # this is used in the plant disease data set to group multiple pictures of
	       # the same leaf.  	      
      		testing_images.append(base_name)
      		result[label_name] = {
	        'dir': dir_name,  
	        'training': training_images,  
	        'testing': testing_images,  
	        'validation': validation_images,  
	      }

    return result


"""Makes sure the folder exists on disk.
Args:
  dir_name: Path string to the folder we want to create.
"""  
def ensure_dir_exists(dir_name):  
  if not os.path.exists(dir_name):  
    os.makedirs(dir_name)  


def cache_bottlenecks(sess, image_lists, image_dir, QF_dir, bottleneck_dir, jpeg_data_tensor, bottleneck_tensor): 
  how_many_bottlenecks = 0  
  ensure_dir_exists(bottleneck_dir)  
  for label_name, label_lists in image_lists.items():  
    for category in ['training', 'testing', 'validation']:  
      category_list = label_lists[category]  
      for index, unused_base_name in enumerate(category_list):  
        get_or_create_bottleneck(sess, image_lists, label_name, index,  
                                 image_dir, QF_dir, category, bottleneck_dir,  
                                 jpeg_data_tensor, bottleneck_tensor)  
  
        how_many_bottlenecks += 1  
        if how_many_bottlenecks % 100 == 0:  
          print(str(how_many_bottlenecks) + ' bottleneck files created.') 


def get_or_create_bottleneck(sess, image_lists, label_name, index, image_dir, QF_dir, category, bottleneck_dir, jpeg_data_tensor, bottleneck_tensor):
  label_lists = image_lists[label_name]  
  sub_dir = label_lists['dir']  
  sub_dir_path = os.path.join(bottleneck_dir, sub_dir)  
  ensure_dir_exists(sub_dir_path)  
  bottleneck_path = get_bottleneck_path(image_lists, label_name, index, bottleneck_dir, category)  
  if not os.path.exists(bottleneck_path):  
    create_bottleneck_file(bottleneck_path, image_lists, label_name, index, image_dir, QF_dir,category, sess, jpeg_data_tensor, bottleneck_tensor)  
  with open(bottleneck_path, 'r') as bottleneck_file:  
    bottleneck_string = bottleneck_file.read()  
  did_hit_error = False  
  try:  
    bottleneck_values = [float(x) for x in bottleneck_string.split(',')]  
  except:  
    print("Invalid float found, recreating bottleneck")  
    did_hit_error = True  
  if did_hit_error:  
    create_bottleneck_file(bottleneck_path, image_lists, label_name, index, image_dir, QF_dir,category, sess, jpeg_data_tensor, bottleneck_tensor)  
    with open(bottleneck_path, 'r') as bottleneck_file:  
      bottleneck_string = bottleneck_file.read()  
    #允许在这里传递异常，因为异常不应该发生在一个新的bottleneck创建之后。  
    bottleneck_values = [float(x) for x in bottleneck_string.split(',')]  
  return bottleneck_values  


def create_bottleneck_file(bottleneck_path, image_lists, label_name, index,  
                           image_dir, QF_dir, category, sess, jpeg_data_tensor, bottleneck_tensor):  
  print('Creating bottleneck at ' + bottleneck_path)  
  image_path = get_image_path(image_lists, label_name, index, image_dir, category)  
  if not gfile.Exists(image_path):  
    tf.logging.fatal('File does not exist %s', image_path)  
  image_data_org = gfile.FastGFile(image_path, 'rb').read()

  # New for 10 QF pictures
  counter = 0;
  image_data_QF = [None]*NUMBER_OF_QUALITY_FACTORS
  for QF in range(START_QF, END_QF, QF_STEP_SIZE):
    image_path_QF = get_image_path_QF(image_lists, label_name, index, QF_dir, category, QF)
    # print (image_path ,'<-->', image_path_QF)
    if not gfile.Exists(image_path):  
      tf.logging.fatal('File does not exist %s', image_path)  
    image_data_QF[counter] = gfile.FastGFile(image_path_QF, 'rb').read()
    counter += 1

  bottleneck_values = run_bottleneck_on_image(sess, image_data_org, image_data_QF, jpeg_data_tensor, bottleneck_tensor)  
  bottleneck_string = ','.join(str(x) for x in bottleneck_values)  
  with open(bottleneck_path, 'w') as bottleneck_file:  
    bottleneck_file.write(bottleneck_string)


def get_image_path(image_lists, label_name, index, image_dir, category):  
  """Returns a path to an image for a label at the given index.

  Args:
    image_lists: OrderedDict of training images for each label.
    label_name: Label string we want to get an image for.
    index: Int offset of the image we want. This will be moduloed by the
    available number of images for the label, so it can be arbitrarily large.
    image_dir: Root folder string of the subfolders containing the training
    images.
    category: Name string of set to pull images from - training, testing, or
    validation.

  Returns:
    File system path string to an image that meets the requested parameters.

  """
  if label_name not in image_lists:  
    tf.logging.fatal('Label does not exist %s.', label_name)  
  label_lists = image_lists[label_name]  
  if category not in label_lists:  
    tf.logging.fatal('Category does not exist %s.', category)  
  category_list = label_lists[category]  
  if not category_list:  
    tf.logging.fatal('Label %s has no images in the category %s.',  
                     label_name, category)  
  mod_index = index % len(category_list)  
  base_name = category_list[mod_index]  
  sub_dir = label_lists['dir']  
  full_path = os.path.join(image_dir, sub_dir, base_name)  
  return full_path

def get_image_path_QF(image_lists, label_name, index, QF_dir, category , QF): 
 """Returns a path to an image for a label at the given index and quality factor.

  Args:
    image_lists: OrderedDict of training images for each label.
    label_name: Label string we want to get an image for.
    index: Int offset of the image we want. This will be moduloed by the
    available number of images for the label, so it can be arbitrarily large.
    QF_dir: Root folder string of the subfolders containing the training
    images with QF.
    category: Name string of set to pull images from - training, testing, or
    validation.
    QF: Quality factor value
  Returns:
    File system path string to an image that meets the requested parameters.

 """

 if label_name not in image_lists:  
  tf.logging.fatal('Label does not exist %s.', label_name)  
 label_lists = image_lists[label_name]  
 if category not in label_lists:  
  tf.logging.fatal('Category does not exist %s.', category)  
 category_list = label_lists[category]  
 if not category_list:  
  tf.logging.fatal('Label %s has no images in the category %s.',  
                     label_name, category)  
 mod_index = index % len(category_list) 
 base_name = category_list[mod_index].split('.')[0]
 middle = '-QF-'+ str(QF)+'.'
 suffix = category_list[mod_index].split('.')[1]
 qf_base_name =  base_name + middle + suffix
 sub_dir = label_lists['dir']
 full_path = os.path.join(QF_dir, sub_dir, qf_base_name)
 return full_path


def get_bottleneck_path(image_lists, label_name, index, bottleneck_dir, category):  
  """Returns a path to a bottleneck file for a label at the given index.

  Args:
    image_lists: OrderedDict of training images for each label.
    label_name: Label string we want to get an image for.
    index: Integer offset of the image we want. This will be moduloed by the
    available number of images for the label, so it can be arbitrarily large.
    bottleneck_dir: Folder string holding cached files of bottleneck values.
    category: Name string of set to pull images from - training, testing, or
    validation.
    module_name: The name of the image module being used.

  Returns:
    File system path string to an image that meets the requested parameters.
  """
  return get_image_path(image_lists, label_name, index, bottleneck_dir,  
                        category) + '.txt'

def run_bottleneck_on_image_QF(sess, bottleneck_values, image_data_QF, image_data_tensor, bottleneck_tensor, qf_counter):
  bottleneck_values_QF = sess.run(bottleneck_tensor, {image_data_tensor: image_data_QF[qf_counter]}) 
  bottleneck_values_QF = np.squeeze(bottleneck_values_QF)
  bottleneck_values_QF = bottleneck_values_QF[0:SIZE_OF_ONE_BOTTLENECK]

  # Concatenate properly:
  start_idx = SIZE_OF_ONE_BOTTLENECK*(1 + qf_counter)
  end_idx   = start_idx + SIZE_OF_ONE_BOTTLENECK
  bottleneck_values[start_idx:end_idx] = bottleneck_values_QF
  return bottleneck_values_QF

  """Runs inference on an image to extract the 'bottleneck' summary layer.
  Args:
    sess: Current active TensorFlow Session.
    image_data_org: String of raw JPEG data.
    image_data_QF: String of JPEG under quantization with a QF.
    image_data_tensor: Input data layer in the graph.
    decoded_image_tensor: Output of initial image resizing and preprocessing.
    resized_input_tensor: The input node of the recognition graph.
    bottleneck_tensor: Layer before the final softmax.

  Returns:
    Numpy array of bottleneck values.
  """ 
def run_bottleneck_on_image(sess, image_data_org, image_data_QF, image_data_tensor, bottleneck_tensor):  

  # ORG
  bottleneck_values = sess.run(bottleneck_tensor, {image_data_tensor: image_data_org})  
  bottleneck_values = np.squeeze(bottleneck_values)  

  # QF versions
  results = Parallel(n_jobs=NUMBER_OF_QUALITY_FACTORS, verbose=0, backend='threading')(delayed(run_bottleneck_on_image_QF)
    (sess, bottleneck_values, image_data_QF, image_data_tensor, bottleneck_tensor, qf_counter) for qf_counter in range(NUMBER_OF_QUALITY_FACTORS))  

  return bottleneck_values  

def main(_):

    # Prepare Inception V3 Graph
    (bottleneck_graph, bottleneck_tensor, jpeg_data_tensor,
     resized_image_tensor) = create_inception_graph()

    if not FLAGS.image_dir:
    	tf.logging.error('Must set flag --imag_dir.')
    	return -1

    if not FLAGS.QF_dir:
    	tf.logging.error('Must set flag --QF_dir.')
    	return -1

    # Create the bottleneck tensor:
    bottleneck_tensor = tf.concat([bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, 
    bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor], 0)
    bottleneck_tensor = tf.reshape(bottleneck_tensor, [1, BOTTLENECK_TENSOR_SIZE])
    # print('New Shape of bottleneck tensor: ', tf.shape(bottleneck_tensor), bottleneck_tensor.get_shape()) 
    # Create the Graph
    # create_graph()

    # bottleneck_tensor = tf.concat([bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor,
    # bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor, bottleneck_tensor], 0)
    # bottleneck_tensor = tf.reshape(bottleneck_tensor, [1, BOTTLENECK_TENSOR_SIZE])

    # Look at the folder structure, and create lists of all the images
    image_lists = create_test_image_lists(FLAGS.image_dir)
    class_count = len(image_lists.keys())

    # Create the session
    sess = tf.Session()

    # We'll make sure we've calculated the 'bottleneck' image summaries and cached them on disk.  
    cache_bottlenecks(sess, image_lists, FLAGS.image_dir, FLAGS.QF_dir, FLAGS.bottleneck_dir,  
                      jpeg_data_tensor, bottleneck_tensor) 


    if class_count == 0:
        print ('No valid folders of images found at ' + FLAGS.image_dir)
        return -1
    if class_count == 1:
        print ('Only one valid folder of images found at ' \
            + FLAGS.image_dir \
            + ' - multiple classes are needed for classification.')
        return -1

    # See if the command-line flags mean we're applying any distortions.
    print ('Stop!')
    return -1

    jpegList = os.listdir(rootdir)
    how_many_sorted_predictions = -5

    for jpegImage in jpegList:
        jpegImageFileName = os.path.join(PATH_TO_TEST_IMAGES, jpegImage)

        # if file exists

        if os.path.isfile(path):

            # Read the image file in Tf lib |decode style of the photo: 'rb' = non-UTF-8; 'r' = UTF-8
            image_data = tf.gfile.FastGFile(jpegImageFileName, 'rb'
                    ).read()

            # ID --> English string label.

        node_lookup = NodeLookup()

        Label = [1, 2, 3]
        top_N = predictions.argsort()[how_many_sorted_predictions:][::
                -1]
        counter = counter + 1
        rank = 0

        for node_id in top_5:
            human_string = node_lookup.id_to_string(node_id)
            score = predictions[node_id]
            print ('%s (score = %.5f)' % (human_string, score))

                # if(Label[counter] == human_string):

    # Close the session
    sess.close()


if __name__ == '__main__':  
  parser = argparse.ArgumentParser()  
  parser.add_argument(  
      '--image_dir',  
      type=str,  
      default='',  
      help='Path to folders of labeled images.'  
  )
  parser.add_argument(  
      '--QF_dir',  
      type=str,  
      default='',  
      help='Path to folders of corresponding QF images.'  
  )
  parser.add_argument(  
      '--output_graph',  
      type=str,  
      default='./v3_mul_imageNet/output_graph.pb',  
      help='Where to save the trained graph.'  
  )  
  parser.add_argument(  
      '--output_labels',  
      type=str,  
      default='./v3_mul_imageNet/output_labels.txt',  
      help='Where to save the trained graph\'s labels.'  
  )  
 
  parser.add_argument(  
      '--print_misclassified_test_images',  
      default=False,  
      help="""
      Whether to print out a list of all misclassified test images.
      """,  
      action='store_true'  
  )  

  parser.add_argument(  
      '--bottleneck_dir',  
      type=str,  
      default='./v3_mul_imageNet_test/bottleneck',  
      help='Path to cache bottleneck layer values as files.'  
  )  
  parser.add_argument(  
      '--final_tensor_name',  
      type=str,  
      default='final_result',  
      help="""\
      The name of the output classification layer in the retrained graph.\
      """  
  )  
  FLAGS, unparsed = parser.parse_known_args()  
tf.app.run(main=main, argv=[sys.argv[0]] + unparsed)  


