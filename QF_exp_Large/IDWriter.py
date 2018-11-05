# -*- coding: utf-8 -*-
# Inception image recognition attempt
import tensorflow as tf
import numpy as np
import re
import os
import time
# import sys
import logging

import xlrd
from xlwt import *
from xlutils.copy import copy
import openpyxl

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' # Hide the warning information from Tensorflow - annoying...

wb = openpyxl.load_workbook('Accuracy Record ILSVRC2012 QF 1000-80.xlsm', read_only = False, keep_vba = True)
sheet=wb['Sheet1']

start_ID = 1;
End_ID = 1001;

######
# Place of identify 
counter = 0

print('Start...')
# Image Range
for ID in range(start_ID,End_ID):
    # Quality Factor Range
    for i in range(1,1009):
        sheet.cell(row=(ID-1)*1009+2+i, column=2).value = i

print('Done...')
# recordnew.save('Accuracy Record ILSVRC2012 QF-100.xls')
wb.save('Accuracy Record ILSVRC2012 QF 1000-number.xlsm')