import numpy as np
from sklearn import svm
from sklearn.utils import shuffle
from sklearn.metrics import accuracy_score

train_data = np.load('dataset/train_data.npy')
train_label = np.load('dataset/train_label.npy')
test_data = np.load('dataset/test_data.npy')
test_label = np.load('dataset/test_label.npy')

print train_data.shape, train_label.shape # (37367, 310) (37367,)
print test_data.shape, test_label.shape # (13588, 310) (13588,)

# split to 1, 0, m1
train_data_1 = []
train_data_0 = []
train_data_m1 = []
for i in range(train_label.shape[0]):
	if (train_label[i] == 1):
		train_data_1.append(train_data[i])
	elif (train_label[i] == 0):
		train_data_0.append(train_data[i])
	else:
		train_data_m1.append(train_data[i])
train_data_1 = np.asarray(train_data_1)
train_data_0 = np.asarray(train_data_0)
train_data_m1 = np.asarray(train_data_m1)
print train_data_1.shape # (12903, 310)
print train_data_0.shape # (12144, 310)
print train_data_m1.shape # (12320, 310)


# --1 vs rest--
print 'training SVMs 1...'
# train_data_1_label
train_label_1 = np.ones((train_data_1.shape[0]))
# the REST training data/label
train_data_1_r = shuffle(np.concatenate((train_data_0, train_data_m1), axis=0))
# partition the REST into arbitrary parts
num_1_r = train_data_0.shape[0] + train_data_m1.shape[0]
part_num = 2 # could be more than 2 in the future
num_1_r_part = int(num_1_r / part_num)
train_data_1_r_part = [[] for i in range(part_num)]
train_label_1_r_part = [[] for i in range(part_num)]
for i in range(part_num - 1):
	train_data_1_r_part[i] = train_data_1_r[num_1_r_part*i : num_1_r_part*(i+1)]
	train_label_1_r_part[i] = np.zeros((num_1_r_part))
train_data_1_r_part[part_num-1] = train_data_1_r[num_1_r_part*(part_num-1):]
train_label_1_r_part[part_num-1] = np.zeros((num_1_r - num_1_r_part * (part_num-1)))
train_data_1_r_part = np.asarray(train_data_1_r_part)
train_label_1_r_part = np.asarray(train_label_1_r_part)
# SVMs
clfs_1 = []
for i in range(part_num):
	clf_1 = svm.SVC(C=1.0, kernel='rbf', gamma='auto', probability=True)
	train_data_1_tmp = np.concatenate((train_data_1, train_data_1_r_part[i]), axis=0)
	train_label_1_tmp = np.concatenate((train_label_1, train_label_1_r_part[i]), axis=0)
	train_data_1_tmp, train_label_1_tmp = shuffle(train_data_1_tmp, train_label_1_tmp)
	clf_1.fit(train_data_1_tmp, train_label_1_tmp)
	clfs_1.append(clf_1)


# --0 vs rest--
print 'training SVMs 0...'
# train_data_1_label
train_label_0 = np.ones((train_data_0.shape[0]))
# the REST training data/label
train_data_0_r = shuffle(np.concatenate((train_data_1, train_data_m1), axis=0))
num_0_r = train_data_1.shape[0] + train_data_m1.shape[0]
# partition the REST into arbitrary parts
part_num = 2 # could be more than 2 in the future
num_0_r_part = int(num_0_r / part_num)
train_data_0_r_part = [[] for i in range(part_num)]
train_label_0_r_part = [[] for i in range(part_num)]
for i in range(part_num - 1):
	train_data_0_r_part[i] = train_data_0_r[num_0_r_part*i : num_0_r_part*(i+1)]
	train_label_0_r_part[i] = np.zeros((num_0_r_part))
train_data_0_r_part[part_num-1] = train_data_0_r[num_0_r_part*(part_num-1):]
train_label_0_r_part[part_num-1] = np.zeros((num_0_r - num_0_r_part * (part_num-1)))
train_data_0_r_part = np.asarray(train_data_0_r_part)
train_label_0_r_part = np.asarray(train_label_0_r_part)
# SVMs
clfs_0 = []
for i in range(part_num):
	clf_0 = svm.SVC(C=1.0, kernel='rbf', gamma='auto', probability=True)
	train_data_0_tmp = np.concatenate((train_data_0, train_data_0_r_part[i]), axis=0)
	train_label_0_tmp = np.concatenate((train_label_0, train_label_0_r_part[i]), axis=0)
	train_data_0_tmp, train_label_0_tmp = shuffle(train_data_0_tmp, train_label_0_tmp)
	clf_0.fit(train_data_0_tmp, train_label_0_tmp)
	clfs_0.append(clf_0)


# --m1 vs rest--
print 'training SVMs m1...'
# train_data_1_label
train_label_m1 = np.ones((train_data_m1.shape[0]))
# the REST training data/label
train_data_m1_r = shuffle(np.concatenate((train_data_1, train_data_0), axis=0))
num_m1_r = train_data_1.shape[0] + train_data_0.shape[0]
# partition the REST into arbitrary parts
part_num = 2 # could be more than 2 in the future
num_m1_r_part = int(num_m1_r / part_num)
train_data_m1_r_part = [[] for i in range(part_num)]
train_label_m1_r_part = [[] for i in range(part_num)]
for i in range(part_num - 1):
	train_data_m1_r_part[i] = train_data_m1_r[num_m1_r_part*i : num_m1_r_part*(i+1)]
	train_label_m1_r_part[i] = np.zeros((num_m1_r_part))
train_data_m1_r_part[part_num-1] = train_data_m1_r[num_m1_r_part*(part_num-1):]
train_label_m1_r_part[part_num-1] = np.zeros((num_m1_r - num_m1_r_part * (part_num-1)))
train_data_m1_r_part = np.asarray(train_data_m1_r_part)
train_label_m1_r_part = np.asarray(train_label_m1_r_part)
# SVMs
clfs_m1 = []
for i in range(part_num):
	clf_m1 = svm.SVC(C=1.0, kernel='rbf', gamma='auto', probability=True)
	train_data_m1_tmp = np.concatenate((train_data_m1, train_data_m1_r_part[i]), axis=0)
	train_label_m1_tmp = np.concatenate((train_label_m1, train_label_m1_r_part[i]), axis=0)
	train_data_m1_tmp, train_label_m1_tmp = shuffle(train_data_m1_tmp, train_label_m1_tmp)
	clf_m1.fit(train_data_m1_tmp, train_label_m1_tmp)
	clfs_m1.append(clf_m1)


# -----MIN-MAX predict-----
# --MIN--
# clfs_1
predict_1_tmp = [[] for i in range(part_num)]
for i in range(len(clfs_1)):
	predict_1_tmp[i] = clfs_1[i].predict_proba(test_data)[:, 1]
predict_1_tmp = np.asarray(predict_1_tmp)
predict_1 = predict_1_tmp.min(0)
# clfs_0
predict_0_tmp = [[] for i in range(part_num)]
for i in range(len(clfs_0)):
	predict_0_tmp[i] = clfs_0[i].predict_proba(test_data)[:, 1]
predict_0_tmp = np.asarray(predict_0_tmp)
predict_0 = predict_0_tmp.min(0)
# clfs_m1
predict_m1_tmp = [[] for i in range(part_num)]
for i in range(len(clfs_m1)):
	predict_m1_tmp[i] = clfs_m1[i].predict_proba(test_data)[:, 1]
predict_m1_tmp = np.asarray(predict_m1_tmp)
predict_m1 = predict_m1_tmp.min(0)
# --MAX--
predict_label = np.argmax(np.concatenate((predict_m1.reshape(-1, 1), predict_0.reshape(-1, 1), predict_1.reshape(-1, 1)), axis=1), axis=1) - 1
print accuracy_score(test_label, predict_label)
