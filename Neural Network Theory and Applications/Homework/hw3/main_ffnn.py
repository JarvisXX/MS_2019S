import os
import time
import numpy as np
import struct
from sklearn.preprocessing import  OneHotEncoder
import tensorflow as tf 


learningRate = 0.01
n_epoch = 1000
# batch_size = 32

n_hidden_1 = 512

def getImage():
	with open('dataset/train-images-idx3-ubyte', 'rb') as file_train_data:
		buf_train_data = file_train_data.read()
	with open('dataset/train-labels-idx1-ubyte', 'rb') as file_train_label:
		buf_train_label = file_train_label.read()
	with open('dataset/t10k-images-idx3-ubyte', 'rb') as file_test_data:
		buf_test_data = file_test_data.read()
	with open('dataset/t10k-labels-idx1-ubyte', 'rb') as file_test_label:
		buf_test_label = file_test_label.read()

	# train data
	index = 0
	index += struct.calcsize('>IIII')
	train_data = []
	for i in range(60000):
		tmp = struct.unpack_from('>784B', buf_train_data, index)
		train_data.append(np.reshape(tmp, (28, 28)))
		index += struct.calcsize('>784B')
	# train label
	index = 0
	index += struct.calcsize('>II')
	train_label = []
	for i in range(60000):
		tmp = struct.unpack_from('>1B', buf_train_label, index)
		train_label.append(tmp)
		index += struct.calcsize('>1B')
	# test data
	index = 0
	index += struct.calcsize('>IIII')
	test_data = []
	for i in range(10000):
		tmp = struct.unpack_from('>784B', buf_test_data, index)
		test_data.append(np.reshape(tmp, (28, 28)))
		index += struct.calcsize('>784B')
	# test label
	index = 0
	index += struct.calcsize('>II')
	test_label = []
	for i in range(10000):
		tmp = struct.unpack_from('>1B', buf_test_label, index)
		test_label.append(tmp)
		index += struct.calcsize('>1B')

	train_data = np.asarray(train_data)
	train_label = np.asarray(train_label)
	test_data = np.asarray(test_data)
	test_label = np.asarray(test_label)

	return train_data, train_label, test_data, test_label


# Multi-Layer Perceptron
def FFNN(x, weights, biases):
	layer_1 = tf.nn.sigmoid(tf.add(tf.matmul(x, weights['h1']), biases['b1']))
	out_layer = tf.nn.sigmoid(tf.add(tf.matmul(layer_1, weights['out']), biases['out']))
	return out_layer


if __name__ == '__main__':
	train_data, train_label, test_data, test_label = getImage()
	# reshape image
	train_data = train_data.reshape((train_data.shape[0], -1))
	test_data = test_data.reshape((test_data.shape[0], -1))
	n_input = train_data.shape[-1]

	# one-hot label
	enc = OneHotEncoder()
	enc.fit(train_label)
	train_label_1hot = enc.transform(train_label).toarray()
	test_label_1hot = enc.transform(test_label).toarray()
	n_classes = train_label_1hot.shape[-1]

	# for i in range(train_data.shape[0]):
	# 	print train_data[i]
	# 	print train_label[i]
	# 	print train_label_1hot[i]
	# 	raw_input()

	# placeholder
	x = tf.placeholder(tf.float32, shape=[None, n_input])
	y = tf.placeholder(tf.float32, shape=[None, n_classes])

	# weights and biases
	weights = {
		'h1': tf.Variable(tf.random_normal([n_input, n_hidden_1])),
		'out': tf.Variable(tf.random_normal([n_hidden_1, n_classes]))
	}

	biases = {
		'b1': tf.Variable(tf.random_normal([n_hidden_1])),
		'out': tf.Variable(tf.random_normal([n_classes]))
	}

	# building model
	predict_y = FFNN(x, weights, biases)

	loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=predict_y, labels=y))
	optimizer = tf.train.AdamOptimizer(learning_rate=learningRate).minimize(loss)

	correct_pred = tf.equal(tf.argmax(predict_y, 1), tf.argmax(y, 1))
	accuracy = tf.reduce_mean(tf.cast(correct_pred, dtype=tf.float32))

	init = tf.global_variables_initializer()

	# # data normalization
	# train_data = normalize(train_data)
	# test_data = normalize(test_data)

	# train & test
	with tf.Session() as sess:
		sess.run(init)

		s = time.time()
		for epoch in range(n_epoch):
			sess.run(optimizer, feed_dict={x:train_data, y:train_label_1hot})
			
			train_loss, train_acc = sess.run([loss, accuracy], feed_dict={x:train_data, y:train_label_1hot})
			print 'epoch: {}, loss: {:.4f}, acc: {:.3f}'.format(epoch, train_loss, train_acc)

		m = time.time()
		test_label_predict = sess.run(predict_y, feed_dict={x:test_data})

		test_acc = sess.run(accuracy, feed_dict={x:test_data, y:test_label_1hot})
		print 'test accuracy: ', test_acc
		e = time.time()
	print 'Training time:', m-s
	print 'Testing time:', e-m
