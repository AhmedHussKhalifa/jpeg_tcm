# -*- coding: utf-8 -*-
# Yanbing Jiang
# Inception image recognition attempt v1
import tensorflow as tf
import numpy as np
import re
import os
import time
# import sys
from tkinter import *
import tkinter.filedialog
from PIL import Image
import matplotlib.pyplot as plt
import logging

import xlrd
from xlwt import *
from xlutils.copy import copy

# Change class ID to human readable label
class NodeLookup(object):
    def __init__(self):  
        label_lookup_path = '../../../5a.Google-Inception/Attempts/inception_model/imagenet_2012_challenge_label_map_proto.pbtxt'   
        uid_lookup_path = '../../../5a.Google-Inception/Attempts/inception_model/imagenet_synset_to_human_label_map.txt'
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

#读取训练好的Inception-v3模型来创建graph
def create_graph():
  # the class that's been created from the textual definition in graph.proto
	with tf.gfile.FastGFile('../../../5a.Google-Inception/Attempts/inception_model/classify_image_graph_def.pb', 'rb') as f:
		graph_def = tf.GraphDef()
		graph_def.ParseFromString(f.read())
		tf.import_graph_def(graph_def, name='')

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' # Hide the warning information from Tensorflow - annoying...

sess=tf.Session()
#create graph
create_graph()


record = xlrd.open_workbook('Accuracy Record ILSVRC2012 Top 5.xls', formatting_info=True)
recordnew = copy(record)
# mySheets = record.sheets()
# 1 Original
# 3 With Enhancement
sheet = recordnew.get_sheet(0)
style = XFStyle()
style.num_format_str = 'general'

###### CHANGE FOR DIFFERENT CASE ######
rootdir = 'C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/jpeg_tcm/QF_exp/' 
file_name = 'frog' ######
file_suffix = '.JPEG' ######
list = os.listdir(rootdir) #列出文件夹下所有的目录与文件

######
# Place of identify 
ID = 8;
counter = 0
for i in range(100,0,-10):
    path =  rootdir + file_name + '-QF-'+ str(i) + file_suffix #os.path.join(rootdir,list[i])
    print(path)
    if os.path.isfile(path):
        # Read the image file in Tf lib |decode style of the photo: 'rb' = non-UTF-8; 'r' = UTF-8
        # image='G:/2018-19Summer/5-Google-Inception/Attempts/images/ele.png'
        image_data = tf.gfile.FastGFile(path, 'rb').read()

        #Inception-v3: last layer is output as softmax
        softmax_tensor= sess.graph.get_tensor_by_name('softmax:0')
        #Input the image, obtain the softmax prob value（one shape=(1,1008) vector）
        predictions = sess.run(softmax_tensor,{'DecodeJpeg/contents:0': image_data})
        #(1,1008)->(1008,)
        predictions = np.squeeze(predictions)

        # ID --> English string label.
        node_lookup = NodeLookup()

        ###### Staring of Comment Place ######
        # Others 
        sheet_r = record.sheets()[0]
        Label = sheet_r.col_values(1)

        top_5 = predictions.argsort()[-2000:][::-1]

        counter = counter + 1
        rank = 0
        for node_id in top_5:
            rank = rank + 1
            human_string = node_lookup.id_to_string(node_id)
            score = predictions[node_id] 
            # print('%s (score = %.5f)' % (human_string, score))
            if(Label[ID] == human_string):
                # -- Original -- 2 3 4 
                print('%d: %s (score = %.5f)' % (i,human_string, score))
                
                sheet.write(counter, 2, rank, style)
                sheet.write(counter, 3, score.item(), style)
                if(rank <= 5):
                    sheet.write(counter, 4, 1, style)
                else:
                    sheet.write(counter, 4, 0, style)
                break

sess.close()
recordnew.save('Accuracy Record ILSVRC2012 Top 5.xls')