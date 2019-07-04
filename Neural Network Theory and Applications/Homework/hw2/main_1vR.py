import numpy as np
from sklearn import svm
from sklearn.metrics import accuracy_score

train_data = np.load('dataset/train_data.npy')
train_label = np.load('dataset/train_label.npy')
test_data = np.load('dataset/test_data.npy')
test_label = np.load('dataset/test_label.npy')

print train_data.shape, train_label.shape # (37367, 310) (37367,)
print test_data.shape, test_label.shape # (13588, 310) (13588,)

# 1 vs rest
print 'training SVM 1...'
train_label_1 = np.zeros((train_label.shape[0]))
for i in range(train_label.shape[0]):
	if (train_label[i] == 1):
		train_label_1[i] = 1
clf_1 = svm.SVC(C=1.0, kernel='rbf', gamma='auto', probability=True)
clf_1.fit(train_data, train_label_1)
print 'SVM 1 classes:', clf_1.classes_ # [0. 1.]

# 0 vs rest
print 'training SVM 0...'
train_label_0 = np.zeros((train_label.shape[0]))
for i in range(train_label.shape[0]):
	if (train_label[i] == 0):
		train_label_0[i] = 1
clf_0 = svm.SVC(C=1.0, kernel='rbf', gamma='auto', probability=True)
clf_0.fit(train_data, train_label_0)
print 'SVM 0 classes:', clf_0.classes_ # [0. 1.]

# -1 vs rest
print 'training SVM m1...'
train_label_m1 = np.zeros((train_label.shape[0]))
for i in range(train_label.shape[0]):
	if (train_label[i] == -1):
		train_label_m1[i] = 1
clf_m1 = svm.SVC(C=1.0, kernel='rbf', gamma='auto', probability=True)
clf_m1.fit(train_data, train_label_m1)
print 'SVM m1 classes:', clf_m1.classes_ # [0. 1.]

# predict
predict_1 = clf_1.predict_proba(test_data)[:, 1]
predict_0 = clf_0.predict_proba(test_data)[:, 1]
predict_m1 = clf_m1.predict_proba(test_data)[:, 1]
predict_label = np.argmax(np.concatenate((predict_m1.reshape(-1, 1), predict_0.reshape(-1, 1), predict_1.reshape(-1, 1)), axis=1), axis=1) - 1
print accuracy_score(test_label, predict_label)

