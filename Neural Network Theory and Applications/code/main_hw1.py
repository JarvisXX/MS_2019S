import os
import numpy as np
from sklearn.preprocessing import normalize, OneHotEncoder
from scipy.io import loadmat
import tensorflow as tf 


# read data
train_data = loadmat('dataset/train_data.mat')['train_data']
train_label = loadmat('dataset/train_label.mat')['train_label']
test_data = loadmat('dataset/test_data.mat')['test_data']
test_label = loadmat('dataset/test_label.mat')['test_label']


# parameters
n_train_sample = train_data.shape[0]
n_test_sample = test_data.shape[0]
n_input = train_data.shape[1]
# n_hidden_1 = 36
n_classes = 3
print '# train sample:', n_train_sample
print '# test sample:', n_test_sample
print 'feature size: ', n_input

# learningRate = 0.01
n_epoch = 1000
# batch_size = 32


# one hot
def OneHot(label):
	label_1 = tf.one_hot(label + 1, n_classes, 1, 0)
	return tf.reshape(label_1, [-1, n_classes])

# Multi-Layer Perceptron
def MLP(x, weights, biases):
	layer_1 = tf.nn.sigmoid(tf.add(tf.matmul(x, weights['h1']), biases['b1']))
	out_layer = tf.nn.sigmoid(tf.add(tf.matmul(layer_1, weights['out']), biases['out']))
	return out_layer


def experiment(train_data, train_label, test_data, test_label, n_hidden_1, learningRate):
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
	predict_y = MLP(x, weights, biases)

	loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=predict_y, labels=y))
	optimizer = tf.train.AdamOptimizer(learning_rate=learningRate).minimize(loss)

	correct_pred = tf.equal(tf.argmax(predict_y, 1), tf.argmax(y, 1))
	accuracy = tf.reduce_mean(tf.cast(correct_pred, dtype=tf.float32))

	init = tf.global_variables_initializer()


	# data normalization
	train_data = normalize(train_data)
	test_data = normalize(test_data)

	# one-hot label
	enc = OneHotEncoder()
	enc.fit(train_label + 1)
	train_label_1hot = enc.transform(train_label + 1).toarray()
	test_label_1hot = enc.transform(test_label + 1).toarray()


	# train & test
	with tf.Session() as sess:
		sess.run(init)

		for epoch in range(n_epoch):
			sess.run(optimizer, feed_dict={x:train_data, y:train_label_1hot})
			
			train_loss, train_acc = sess.run([loss, accuracy], feed_dict={x:train_data, y:train_label_1hot})
			# print 'epoch: {}, loss: {:.4f}, acc: {:.3f}'.format(epoch, train_loss, train_acc)

		test_label_predict = sess.run(predict_y, feed_dict={x:test_data})

		test_acc = sess.run(accuracy, feed_dict={x:test_data, y:test_label_1hot})
		# print 'test accuracy: ', test_acc
		return train_loss, train_acc, test_acc


h_unit = np.array([15, 50, 100, 150, 200])
l_rate = np.array([0.1, 0.01, 0.001])
train_loss_array = np.zeros((5,3))
train_acc_array = np.zeros((5,3))
test_acc_array = np.zeros((5,3))
for i in range(5):
	for j in range(3):
		train_loss_array[i][j], train_acc_array[i][j], test_acc_array[i][j] = experiment(train_data, train_label, test_data, test_label, h_unit[i], l_rate[j])
for i in range(5):
	for j in range(3):
		print '# hidden units: %-8d learning rate: %-8g training loss: %-8.4f training accuracy: %-8.4f testing accuracy: %-8.4f' % (h_unit[i], l_rate[j], train_loss_array[i][j], train_acc_array[i][j], test_acc_array[i][j])

