import tensorflow as tf 
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

def OneHot(label):
	label_1 = tf.one_hot(label + 1, n_classes, 1, 0)
	return tf.reshape(label_1, [-1, n_classes])

x = tf.placeholder(tf.float32, shape=[None, n_input])
y = tf.placeholder(tf.float32, shape=[None, n_classes])

weights = {
	'h1': tf.Variable(tf.random_normal([n_input, n_hidden_1])),
	'out': tf.Variable(tf.random_normal([n_hidden_1, n_classes]))
}

biases = {
	'b1': tf.Variable(tf.random_normal([n_hidden_1])),
	'out': tf.Variable(tf.random_normal([n_classes]))
}

def MLP(x, weights, biases):
	layer_1 = tf.nn.sigmoid(tf.add(tf.matmul(x, weights['h1']), biases['b1']))
	out_layer = tf.nn.sigmoid(tf.add(tf.matmul(layer_1, weights['out']), biases['out']))
	return out_layer

predict_y = MLP(x, weights, biases)

loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=predict_y, labels=y))
optimizer = tf.train.AdamOptimizer(learning_rate=learningRate).minimize(loss)

correct_pred = tf.equal(tf.argmax(predict_y, 1), tf.argmax(y, 1))
accuracy = tf.reduce_mean(tf.cast(correct_pred, dtype=tf.float32))

init = tf.global_variables_initializer()

with tf.Session() as sess:
	sess.run(init)
	# total_batch = int(n_sample / batch_size)
	train_label_1hot = sess.run(OneHot(train_label))
	test_label_1hot = sess.run(OneHot(test_label))

	for epoch in range(n_epoch):
		sess.run(optimizer, feed_dict={x:train_data, y:train_label_1hot})
		
		train_loss, train_acc = sess.run([loss, accuracy], feed_dict={x:train_data, y:train_label_1hot})
		print 'epoch: {}, loss: {:.4f}, acc: {:.3f}'.format(epoch, train_loss, train_acc)

	test_label_predict = sess.run(predict_y, feed_dict={x:test_data})

	test_acc = sess.run(accuracy, feed_dict={x:test_data, y:test_label_1hot})
	print 'test accuracy: ', test_acc

for test_num in xrange(len(test_data)):
	print test_label[test_num]
	print test_label_1hot[test_num]
	print test_label_predict[test_num]


