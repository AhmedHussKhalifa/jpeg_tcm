'''Trains a simple convnet on the MNIST dataset.
Gets to 99.25% test accuracy after 12 epochs
(there is still a lot of margin for parameter tuning).
16 seconds per epoch on a GRID K520 GPU.
'''

from __future__ import print_function
import keras
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
from keras import backend as K

from sklearn.metrics import classification_report, confusion_matrix
from sklearn.preprocessing import LabelEncoder
from sklearn.preprocessing import OneHotEncoder
from sklearn.preprocessing import normalize

import random
import numpy as np
import pandas as pd


from sklearn import preprocessing

from sklearn.utils import class_weight

from keras import optimizers




def load_csv(filename):
  # Read data from file 'filename.csv' 
  # (in the same directory that your python process is based)
  # Control delimiters, rows, column names with read_csv (see later) 
  data = pd.read_csv(filename, skiprows=1, skipinitialspace=True) 
  # Preview the first 5 lines of the loaded data 
  # print(data.head())
  return data

def getAnchorTop1Rate(y):
    zero_count = np.count_nonzero(y==0);
    total_size = y.shape[0]
    anchor_top1_rate = 100.0*zero_count/total_size;
    return zero_count, anchor_top1_rate

def getMyTop1Rate(confusion_matrix):
    my_zero_count = confusion_matrix[0,0] + confusion_matrix[1,1]
    my_total_size = np.sum(confusion_matrix)
    my_top1_rate = 100.0*my_zero_count/my_total_size;
    return my_zero_count, my_top1_rate 

def getConfusionMatrix(y, preds):

    # print (type(y))
    # print(type(preds))
    # print (y.shape)
    # print (preds.shape)
    confusionMatrix = np.array([ [ np.count_nonzero(np.logical_and(y==0, preds==0)), np.count_nonzero(np.logical_and(y==1, preds==0)) ],
                        [np.count_nonzero(np.logical_and(y==0, preds==1)), np.count_nonzero(np.logical_and(y==1, preds==1))]])
    
    return confusionMatrix



# Cross validation undone and unused yet
def load_data_kfold(k):
    
    train = pd.read_json('../input/train.json')
    train.inc_angle = train.inc_angle.replace('na', 0)
    
    x_band1 = np.array([np.array(band).astype(np.float32).reshape(75, 75) for band in train["band_1"]])
    x_band2 = np.array([np.array(band).astype(np.float32).reshape(75, 75) for band in train["band_2"]])
    x_band3 = x_band1 / x_band2
       
    X_train = np.concatenate([x_band1[:, :, :, np.newaxis]
                            , x_band2[:, :, :, np.newaxis]
                            , x_band3[:, :, :, np.newaxis]], axis=-1)
                         
    y_train = np.array(train["is_iceberg"])
    
    folds = list(StratifiedKFold(n_splits=k, shuffle=True, random_state=1).split(X_train, y_train))
    
    return folds, X_train, y_train


batch_size = 128
num_classes = 2
epochs = 20
is_enable_input_preprocess = 0


# input_file_name = '/Users/hossam.amer/7aS7aS_Works/work/jpeg_ml_research/Competing_collection_17k_last.csv'
input_file_name = '/Users/hossam.amer/7aS7aS_Works/work/jpeg_ml_research/Competing_collection_25k.csv'
# input_file_name = '/Users/hossam.amer/7aS7aS_Works/work/jpeg_ml_research/Competing_collection_25k_stat.csv'

# Import Data:
# -------------

print("Loading Input CSV file...")

input_data = load_csv(input_file_name) # Dataframe

nTrainingSamples = input_data.shape[0]
nFeatures = input_data.shape[1]



# input_data['id'] = [random.randint(0, nTrainingSamples) for x in range(input_data.shape[0])]
x_train = input_data.iloc[:, 0:nFeatures-1]
# x_train = input_data.iloc[:, 22:nFeatures-1]
# x_train = input_data.iloc[:, 0:22-1]
y_train = input_data.iloc[:, nFeatures-1]



# Input preprocessing: (better not to do it given this mix)
# scaler = preprocessing.StandardScaler()
# # Fit your data on the scaler object
# scaled_df = scaler.fit_transform(x_train)
# x_train = pd.DataFrame(scaled_df)


# One hot representation
# binary encode
# label_encoder   = LabelEncoder()
# integer_encoded = label_encoder.fit_transform(y_train)
# # print(integer_encoded)

# onehot_encoder = OneHotEncoder(sparse=False)
# integer_encoded = integer_encoded.reshape(len(y_train), 1)
# y_train = onehot_encoder.fit_transform(integer_encoded)
# print(y_train)

# y_train = input_data.iloc[:, nFeatures-1:nFeatures]

# # Convert into numpy array
x_train = x_train.values

if is_enable_input_preprocess:
  # Input preprocessing:
  x_train_scaled = preprocessing.scale(x_train)
  x_train = x_train_scaled

#  Get the top-1 rate stats before converting into 1-hot rep
y_train_not_one_hot = y_train
# anchor_zero_count, anchor_top1_rate = getAnchorTop1Rate(y_train_not_one_hot)



# Convert into one-hot representation
y_train = keras.utils.to_categorical(y_train, num_classes)

# Test data (Equal to x_train)
x_test = x_train
y_test = y_train


# 0.7 split ratio
X = x_train
y = y_train
ratio = 0.9
n = int(ratio*nTrainingSamples)
x_train = X[1:n,:]
y_train = y[1:n]
x_test = X[n:nTrainingSamples,:]
y_test = y[n:nTrainingSamples]
y_test_not_one_hot = y_train_not_one_hot[n:nTrainingSamples]

cTrain = getConfusionMatrix(np.argmax(y_train, axis=1), np.argmax(y_train, axis=1))
cTest  = getConfusionMatrix(np.argmax(y_test, axis=1),  np.argmax(y_test,  axis=1))

anchor_zero_count = cTrain[0, 0]
anchor_top1_rate  = 100*cTrain[0, 0]/np.sum(cTrain)

anchor_test_zero_count = cTest[0, 0]
anchor_test_top1_rate  = 100*cTest[0, 0]/np.sum(cTest)


print (cTrain)
print('ORG Training Top1 Rate', 100*cTrain[0, 0]/np.sum(cTrain))
print (cTest)
print('ORG Test Top1 Rate', 100*cTest[0, 0]/np.sum(cTest))


# -------------

# Checks on types
# print (x_train.shape)
# print (y_train.shape)
# print (type(x_train))
# print (type(y_train))

print('-------------------')
print (nFeatures, ' features')
print(x_train.shape[0], 'train samples')
print(x_test.shape[0], 'test samples')
print('Anchor Training Top-1 zero_count: ', anchor_zero_count,  ', and Anchor Training Top-1 Rate: ', anchor_top1_rate)
portion_0 = anchor_top1_rate
portion_1 = 100-anchor_top1_rate
print('Class 0 portion size: ' , portion_0)
print('Class 1 portion size: ' , portion_1)
print('-------------------')

# Create the model:
#------------------

model = Sequential()
# model.add(Dense(1024, activation='relu'))
# model.add(Dense(800, activation='relu'))
# model.add(Dense(600, activation='relu'))
model.add(Dense(500, activation='relu'))
model.add(Dropout(0.5))
model.add(Dense(num_classes, activation='softmax'))

# model.add(Dense(1024, activation='relu'))
# model.add(Dropout(0.5))
# model.add(Dense(num_classes, activation='softmax'))

# For trials:
# model = Sequential()
# model.add(Dense(512, input_shape=(784,)))
# model.add(Activation('relu'))
# model.add(Dropout(0.2))
# model.add(Dense(512))
# model.add(Activation('relu'))
# model.add(Dropout(0.2))
# model.add(Dense(10))
# model.add(Activation('softmax'))

# model.compile(loss=keras.losses.categorical_crossentropy,
#               optimizer=keras.optimizers.Adadelta(),
#               metrics=['accuracy'])

# Model with new loss function including class weights
# class_weight = {0: portion_0,
#                 1: portion_1}
# class_weights = {0: 1.0,
#                 1: portion_0}
# class_weights = {0: 1.0,
#                 1: 100.0}

class_weights = class_weight.compute_class_weight('balanced',
                                                 np.unique(y_train_not_one_hot),
                                                 y_train_not_one_hot)
print (class_weights)


# Less learning rate by one decimal place than defualt:
sgd = optimizers.SGD(lr=0.001, decay=1e-6, momentum=0.9, nesterov=True)
# sgd = keras.optimizers.Adam(lr=0.0001, beta_1=0.9, beta_2=0.999, epsilon=None, decay=0.0, amsgrad=False)


model.compile(loss=keras.losses.categorical_crossentropy,
              optimizer=sgd,
              metrics=['accuracy'], class_weight=class_weights)
# model.compile(loss=keras.losses.categorical_crossentropy,
#               optimizer='adam',
#               metrics=['accuracy'], class_weight=class_weights)

# model.compile(loss=keras.losses.categorical_crossentropy,
#               optimizer=keras.optimizers.Adadelta(),
#               metrics=['accuracy'])



model.fit(x_train, y_train,
          batch_size=batch_size,
          epochs=epochs,
           verbose=1,
           validation_data=(x_test, y_test)) # you can shuffle here

# Output the summary
model.summary()

score = model.evaluate(x_test, y_test, verbose=0)
print('Test loss:', score[0])
print('Test accuracy:', score[1])


# Test data:
# Confution Matrix and Classification Report
Y_pred = model.predict(np.array(x_test))
y_pred = np.argmax(Y_pred, axis=1)


print('Test ORG Confusion Matrix')
print(cTest)
print('Test NN Confusion Matrix')
# confusion_matrix = getConfusionMatrix(y_train_not_one_hot.values, y_pred)
confusion_matrix = getConfusionMatrix(y_test_not_one_hot.values, y_pred)
print(confusion_matrix)


test_zero_count, test_top1_rate = getMyTop1Rate(confusion_matrix)
print('Anchor Test Top-1 zero_count: ', anchor_test_zero_count,  ', and Anchor Test Top-1 Rate: ', anchor_test_top1_rate)
print('NN Test Top-1 zero_count: ', test_zero_count,  ', and NN Test Top-1 Rate: ', test_top1_rate)



print('Anchor Total Number of pictures detected: ', (cTrain[0,0] + cTest[1,1]))
print('NN Test Total Number of pictures detected: ', (confusion_matrix[0,0] + confusion_matrix[1,1]))

# Extract the weights
# for layer in model.layers:
#     weights = layer.get_weights() # list of numpy arrays
#     print (weights)
#     print(type(weights))
#     print (len(weights))
#     print ('---------------------')

# # Save model and weights
# save_dir = os.path.join(os.getcwd(), 'saved_models')
# model_name = 'keras_cifar10_trained_model.h5'
# if not os.path.isdir(save_dir):
#     os.makedirs(save_dir)
# model_path = os.path.join(save_dir, model_name)
# model.save(model_path)
# print('Saved trained model at %s ' % model_path)

