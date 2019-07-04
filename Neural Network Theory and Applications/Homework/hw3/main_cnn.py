import os
import time
import numpy as np
import struct
from sklearn.preprocessing import OneHotEncoder
import tensorflow as tf 
from draw_fig import draw_image
from draw_fig import draw_feature


learningRate = 0.005
n_epoch = 1000
# batch_size = 32

# conv 1
CONV1_SIZE = 5
N_CHANNEL_1 = 1
N_KERNEL_1 = 6
# conv 2
CONV2_SIZE = 5
N_CHANNEL_2 = 6
N_KERNEL_2 = 16
# FC 1
N_FC_0 = 784
N_FC_1 = 120
N_FC_2 = 84
N_CLASS = 10

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

def getWeights():
	weights = {
		'conv1': tf.Variable(tf.truncated_normal([CONV1_SIZE, CONV1_SIZE, N_CHANNEL_1, N_KERNEL_1])),
		'conv2': tf.Variable(tf.truncated_normal([CONV2_SIZE, CONV2_SIZE, N_CHANNEL_2, N_KERNEL_2])),
		'fc1': tf.Variable(tf.truncated_normal([N_FC_0, N_FC_1])),
		'fc2': tf.Variable(tf.truncated_normal([N_FC_1, N_FC_2])),
		'fc3': tf.Variable(tf.truncated_normal([N_FC_2, N_CLASS]))
	}
	return weights


def getBiases():
	biases = {
		'conv1': tf.Variable(tf.truncated_normal([N_KERNEL_1])),
		'conv2': tf.Variable(tf.truncated_normal([N_KERNEL_2])),
		'fc1': tf.Variable(tf.truncated_normal([N_FC_1])),
		'fc2': tf.Variable(tf.truncated_normal([N_FC_2])),
		'fc3': tf.Variable(tf.truncated_normal([N_CLASS]))
	}
	return biases


if __name__ == '__main__':
	train_data, train_label, test_data, test_label = getImage()
	# reshape image
	train_data = train_data.reshape((train_data.shape[0], train_data.shape[1], train_data.shape[2], N_CHANNEL_1))
	test_data = test_data.reshape((test_data.shape[0], test_data.shape[1], test_data.shape[2], N_CHANNEL_1))
	im_size = train_data.shape[1]

	# one-hot label
	enc = OneHotEncoder(categories='auto')
	enc.fit(train_label)
	train_label_1hot = enc.transform(train_label).toarray()
	test_label_1hot = enc.transform(test_label).toarray()
	n_classes = train_label_1hot.shape[-1]

	print train_data.shape
	print train_label.shape
	print test_data.shape
	print test_label.shape

	# placeholder
	x = tf.placeholder(tf.float32, shape=[None, im_size, im_size, N_CHANNEL_1])
	y = tf.placeholder(tf.float32, shape=[None, n_classes])

	# weights and biases
	weights = getWeights()
	biases = getBiases()

	# building model
	conv_1 = tf.nn.sigmoid(tf.nn.conv2d(x, weights['conv1'], strides=[1,1,1,1], padding="SAME"))
	pool_1 = tf.nn.max_pool(conv_1, ksize=[1,2,2,1], strides=[1,2,2,1], padding="SAME")
	conv_2 = tf.nn.sigmoid(tf.nn.conv2d(pool_1, weights['conv2'], strides=[1,1,1,1], padding="SAME"))
	pool_2 = tf.nn.max_pool(conv_2, ksize=[1,2,2,1], strides=[1,2,2,1], padding="SAME")

	pool_2_rs = tf.reshape(pool_2, [-1, N_FC_0])

	fc_1 = tf.nn.sigmoid(tf.add(tf.matmul(pool_2_rs, weights['fc1']), biases['fc1']))
	fc_2 = tf.nn.sigmoid(tf.add(tf.matmul(fc_1, weights['fc2']), biases['fc2']))
	predict_y = tf.nn.sigmoid(tf.add(tf.matmul(fc_2, weights['fc3']), biases['fc3']))

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

		hidden_feature_1 = sess.run(conv_1, feed_dict={x:test_data})
		hidden_feature_2 = sess.run(conv_2, feed_dict={x:test_data})
		file_new = open('label.txt', 'w')
		for i in range(10):
			draw_image('image_'+str(i), test_data[i], 1)
			draw_feature('feature_'+str(i)+'_conv1', hidden_feature_1[i], 1)
			draw_feature('feature_'+str(i)+'_conv2', hidden_feature_2[i], 1)
			file_new.write(str(test_label[i][0]))
			file_new.write('\t')
			file_new.write(str(np.argmax(test_label_predict[i])))
			file_new.write('\n')
		file_new.close()

	print 'Training time:', m-s
	print 'Testing time:', e-m
