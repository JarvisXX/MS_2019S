import os
import numpy as np
from scipy.io import loadmat
from sklearn.preprocessing import normalize, OneHotEncoder
from sklearn import svm
from sklearn.metrics import accuracy_score
from sklearn.decomposition import PCA


# read data
X = loadmat('dataset/EEG_X.mat')['X'][0]
Y = loadmat('dataset/EEG_Y.mat')['Y'][0]

n_domain = X.shape[0] # 15
# In each subject, there are 3394 samples with feature dimensionality being 310.


acc_array = []
# 15-fold cross-validation
for i in range(n_domain):
	# i is the left-out subject
	print '----------Subject ' + str(i) + ' is left out for testing----------'

	# extract training & testing data
	train_data = np.zeros((0, 310))
	train_label = np.zeros((0, 1))
	for j in range(n_domain):
		if (j == i):
			test_data = X[j]
			test_label = Y[j]
		else:
			train_data = np.concatenate((train_data, X[j]))
			train_label = np.concatenate((train_label, Y[j]))
	train_label = train_label.ravel()
	test_label = test_label.ravel()

	# print train_data.shape # (47516, 310)
	# print train_label.shape # (47516)
	# print test_data.shape # (3994, 310)
	# print test_label.shape # (3994)

	# PCA
	enPCA = 1
	if enPCA == 1:
	    pca = PCA(n_components=50)
	    pca.fit(train_data)
	    train_data = pca.transform(train_data)
	    test_data = pca.transform(test_data)
	    print 'After PCA:', train_data.shape, test_data.shape

	# SVM
	clf = svm.SVC(C=1.0, kernel='rbf', gamma='auto')
	clf.fit(train_data, train_label)
	predict_label = clf.predict(test_data)
	acc = accuracy_score(test_label, predict_label)
	print 'Accuracy:', acc
	acc_array.append(acc)

print np.mean(acc_array)
