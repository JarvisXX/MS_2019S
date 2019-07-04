import numpy as np
from scipy.io import loadmat
from scipy.linalg import eig
from sklearn.metrics import accuracy_score
from sklearn.neighbors import KNeighborsClassifier
from sklearn.decomposition import PCA


def kernel(ker, X, X2, gamma):
    if not ker or ker == 'primal':
        return X
    elif ker == 'linear':
        if not X2:
            K = np.dot(X.T, X)
        else:
            K = np.dot(X.T, X2)
    elif ker == 'rbf':
        n1sq = np.sum(X ** 2, axis=0)
        n1 = X.shape[1]
        if not X2:
            D = (np.ones((n1, 1)) * n1sq).T + np.ones((n1, 1)) * n1sq - 2 * np.dot(X.T, X)
        else:
            n2sq = np.sum(X2 ** 2, axis=0)
            n2 = X2.shape[1]
            D = (np.ones((n2, 1)) * n1sq).T + np.ones((n1, 1)) * n2sq - 2 * np.dot(X.T, X)
        K = np.exp(-gamma * D)
    elif ker == 'sam':
        if not X2:
            D = np.dot(X.T, X)
        else:
            D = np.dot(X.T, X2)
        K = np.exp(-gamma * np.arccos(D) ** 2)
    return K


class TCA:
    def __init__(self, kernel_type='primal', dim=30, lamb=1, gamma=1):
        '''
        Init func
        :param kernel_type: kernel, values: 'primal' | 'linear' | 'rbf' | 'sam'
        :param dim: dimension after transfer
        :param lamb: lambda value in equation
        :param gamma: kernel bandwidth for rbf kernel
        '''
        self.kernel_type = kernel_type
        self.dim = dim
        self.lamb = lamb
        self.gamma = gamma

    def fit(self, Xs, Xt):
        '''
        Transform Xs and Xt
        :param Xs: ns * n_feature, source feature
        :param Xt: nt * n_feature, target feature
        :return: Xs_new and Xt_new after TCA
        '''
        X = np.hstack((Xs.T, Xt.T))
        X /= np.linalg.norm(X, axis=0)
        m, n = X.shape
        ns, nt = len(Xs), len(Xt)
        e = np.vstack((1 / ns * np.ones((ns, 1)), -1 / nt * np.ones((nt, 1))))
        M = e * e.T
        M = M / np.linalg.norm(M, 'fro')
        H = np.eye(n) - 1 / n * np.ones((n, n))
        K = kernel(self.kernel_type, X, None, gamma=self.gamma)
        n_eye = m if self.kernel_type == 'primal' else n
        a, b = np.linalg.multi_dot([K, M, K.T]) + self.lamb * np.eye(n_eye), np.linalg.multi_dot([K, H, K.T])
        w, V = eig(a, b)
        ind = np.argsort(w)
        A = V[:, ind[:self.dim]]
        Z = np.dot(A.T, K)
        Z /= np.linalg.norm(Z, axis=0)
        Xs_new, Xt_new = Z[:, :ns].T, Z[:, ns:].T
        return Xs_new, Xt_new

    def fit_predict(self, Xs, Ys, Xt, Yt):
        '''
        Transform Xs and Xt, then make predictions on target using 1NN
        :param Xs: ns * n_feature, source feature
        :param Ys: ns * 1, source label
        :param Xt: nt * n_feature, target feature
        :param Yt: nt * 1, target label
        :return: Accuracy and predicted_labels on the target domain
        '''
        Xs_new, Xt_new = self.fit(Xs, Xt)
        clf = KNeighborsClassifier(n_neighbors=1)
        clf.fit(Xs_new, Ys.ravel())
        y_pred = clf.predict(Xt_new)
        acc = accuracy_score(Yt, y_pred)
        return acc, y_pred


if __name__ == '__main__':
    X = loadmat('dataset/EEG_X.mat')['X'][0]
    Y = loadmat('dataset/EEG_Y.mat')['Y'][0]

    n_domain = X.shape[0]

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
				train_tmp = np.concatenate((X[j], np.reshape(Y[j], (-1, 1))), axis=1)
				np.random.shuffle(train_tmp)
				train_tmp = train_tmp[:200]
				train_data = np.concatenate((train_data, train_tmp[:,:-1]))
				train_label = np.concatenate((train_label, np.reshape(train_tmp[:,-1], (-1, 1))))
    	train_label = train_label.ravel()
    	test_label = test_label.ravel()

        # train_data = train_data[:4000]
        # train_label = train_label[:4000]

        #print train_data.shape # (47516, 310)
        #print train_label.shape # (47516)
        #print test_data.shape # (3994, 310)
        #print test_label.shape # (3994)

        # down-sampling training
        #train_tmp = np.concatenate((train_data, np.reshape(train_label, (-1, 1))), axis=1)
        #np.random.shuffle(train_tmp)
        #n_trian_sample = 20000
        #train_tmp = train_tmp[:n_trian_sample]
        #train_data = train_tmp[:,:-1]
        #train_label = train_tmp[:,-1]
        del train_tmp
        # down-sampling testing
        test_tmp = np.concatenate((test_data, np.reshape(test_label, (-1, 1))), axis=1)
        np.random.shuffle(test_tmp)
        n_test_sample = 2000
        test_tmp = test_tmp[:n_test_sample]
        test_data = test_tmp[:,:-1]
        test_label = test_tmp[:,-1]
        del test_tmp

        # print train_data.shape # (7000, 310)
        # print train_label.shape # (7000)
        # print test_data.shape # (3994, 310)
        # print test_label.shape # (3994)

    	# TCA
    	tca = TCA(kernel_type='rbf', dim=100, lamb=1, gamma=1)
    	acc, y_pre = tca.fit_predict(train_data, train_label, test_data, test_label)
    	print 'Accuracy:', acc
    	acc_array.append(acc)

    print np.mean(acc_array)


# 2000 - 2000: 54.78%
# tmux a -t 0
#   dim = 50
# tmux a -t 1
#   dim = 100
