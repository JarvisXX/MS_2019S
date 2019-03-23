import tensorflow as tf 
from keras.models import Sequential, Model
from keras.layers import Input, Dense
from keras.utils import to_categorical
import os
import numpy as np
from scipy.io import loadmat

train_data = loadmat('dataset/train_data.mat')['train_data']
train_label = loadmat('dataset/train_label.mat')['train_label']
test_data = loadmat('dataset/test_data.mat')['test_data']
test_label = loadmat('dataset/test_label.mat')['test_label']

n_sample = train_data.shape[0]
n_input = train_data.shape[1]
n_hidden_1 = 100
n_classes = 3

learningRate = 0.01
n_epoch = 10
# batch_size = 32


input_1 = Input(shape=(n_input,), name='input_1')
dense_1 = Dense(n_hidden_1, activation='sigmoid', name='hidden_1')(input_1)
dense_2 = Dense(n_classes, activation='sigmoid', name='output_1')(dense_1)
model = Model(inputs=[input_1], outputs=[dense_2])
model.compile(loss='categorical_crossentropy',
			  optimizer='adam',
			  metrics=['accuracy'])

train_label_1hot = to_categorical(train_label + 1)
test_label_1hot = to_categorical(test_label + 1)

model.fit({'input_1': train_data}, {'output_1': train_label_1hot}, epochs=n_epoch, shuffle=True, batch_size=32, validation_data=(test_data, test_label_1hot))

print model.evaluate(test_data, test_label_1hot, batch_size=32)

# --------------------evaluate--------------------
print 'evaluating...'
for test_num in xrange(len(test_data)):
	# print test_data[test_num]
	print model.predict(np.asarray([test_data[test_num]]))
	print test_label[test_num]
	print test_label_1hot[test_num]


