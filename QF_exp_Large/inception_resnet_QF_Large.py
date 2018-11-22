# -*- coding: utf-8 -*-
import numpy as np
import time
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2' # Hide the warning information from Tensorflow - annoying...
import tensorflow as tf
import sys  
sys.path.append(r"C:/Users/y77jiang/AppData/Local/Programs/Python/Python36/models/research/slim"); 
try:
    import urllib2 as urllib
except ImportError:
    import urllib.request as urllib

from datasets import imagenet
from nets import inception
from preprocessing import inception_preprocessing
import xlrd
from xlwt import *
from xlutils.copy import copy

def is_image(image):
    if image != '':
        print("The image you chose is： \n" + image)
        parsed_path = image.split('/')
        parsed_name = parsed_path[-1].split('.')
        # print (parsed_name[-1])
        if (not parsed_name[-1] == 'png') and (not parsed_name[-1] == 'jpg') and (not parsed_name[-1] == 'jpeg') and (not parsed_name[-1] == 'JPEG') and (not parsed_name[-1] == 'bmp'): 
            print('Your choice is not a picture.')
            return False
        else:
            return True
    else:
        print("You did not select any picture file.")
        return False

slim = tf.contrib.slim

# print('waiting...')
# time.sleep(19800)

batch_size = 3
image_size = inception.inception_v3.default_image_size
checkpoints_dir = 'C:/Users/y77jiang/Downloads'
checkpoints_filename = 'inception_resnet_v2_2016_08_30.ckpt'
model_name = 'InceptionResnetV2'
graph = tf.Graph()
graph.as_default()

record = xlrd.open_workbook('Accuracy Record ILSVRC2012 QF V4 MAXX.xls', formatting_info=True)
recordnew = copy(record)
# mySheets = record.sheets()
# 3 Inception Resnet V2
sheet = recordnew.get_sheet(0)
style = XFStyle()
style.num_format_str = 'general'
sheet_r = record.sheets()[0]
Label = sheet_r.col_values(1)
###### CHANGE FOR DIFFERENT CASE ######
# Group Test
rootdir = 'C:/Users/y77jiang/Downloads/ILSVRC2012/7/'
file_name_initial = 'ILSVRC2012_val_'
# file_suffix = '.jpg';
file_suffix = '.JPEG';
list = os.listdir(rootdir) # List all file in the folder
start_ID = 1;
End_ID = 1001;


for ID in range(start_ID,End_ID):
    file_name_number = str(ID).zfill(8) 
    file_name = file_name_initial + file_name_number

    # Quality Factor Range
    for j in range(20,11,-10):
        sess = tf.InteractiveSession()
        path =  rootdir + file_name + '-QF-'+ str(j) + file_suffix #os.path.join(rootdir,list[i])
        if os.path.isfile(path):
            # print(path)
            url = 'file:///'+ path
            image_string = urllib.urlopen(url).read()
            parsed_path = url.split('/')
            parsed_name = parsed_path[-1].split('.')

            # print(parsed_name[-1])

            if  parsed_name[-1] == 'bmp': 
                image = tf.image.decode_bmp(image_string, channels=3)
            else:
                image = tf.image.decode_jpeg(image_string, channels=3)

            processed_image = inception_preprocessing.preprocess_image(image, image_size, image_size, is_training=False)
            processed_images  = tf.expand_dims(processed_image, 0)

            # Create the model, use the default arg scope to configure the batch norm parameters.
            with slim.arg_scope(inception.inception_resnet_v2_arg_scope()):
                logits, _ = inception.inception_resnet_v2(processed_images, num_classes=1001, is_training=False)
                probabilities = tf.nn.softmax(logits)

            init_fn = slim.assign_from_checkpoint_fn(os.path.join(checkpoints_dir, checkpoints_filename), slim.get_model_variables(model_name))

            init_fn(sess)
            np_image, probabilities = sess.run([image, probabilities])
            probabilities = probabilities[0, 0:]
            sorted_inds = [i[0] for i in sorted(enumerate(-probabilities), key=lambda x:x[1])]

            names = imagenet.create_readable_names_for_imagenet_labels()

            ###### Staring of Comment Place ######
            # Others 

            for i in range(0,1000):
                index = sorted_inds[i]
                if(Label[ID] == names[index]):
                    # -- Original -- 2 3 4 
                    print('%d - %d. %0.5f%% => [%s], rank = %d' % (ID, j,probabilities[index], names[index], i+1))
                    sheet.write(ID, 2, i+1, style)
                    sheet.write(ID, 3, probabilities[index].item(), style)
                    break

        tf.reset_default_graph()
        sess.close()


    recordnew.save('Accuracy Record ILSVRC2012 QF V4 MAXX - 20.xls')