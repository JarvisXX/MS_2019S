X = load('EEG_X.mat');
Y = load('EEG_Y.mat');
X = X.X;
Y = Y.Y;

num_per_class = size(cell2mat(Y(1)),1);
X_mat = [];
Y_mat = [];
for i = 1:15
    X_mat = cat(1, X_mat, cell2mat(X(i)));
    Y_mat = cat(1, Y_mat, cell2mat(Y(i)));
end

for i = 1:15
    fprintf('Subject %d is left out for testing.\n', i);
    maSrc = true(size(Y_mat));
    maSrc((i-1)*num_per_class+1:i*num_per_class) = false;
    % [ftAllNew,transMdl] = ftTrans_tca(ftAll,maSrc,target,maLabeled,param);
    % ftAll: All samples in all domains. n-by-m matrix, n is the number of samples, m is the dimension of features.
    % maSrc: n-by-1 logical vector, maSrc(i)=true if i is from the source domain, 0 if target domain.
    % target: When some samples in any domain are labeled, their labels can be provided in this variable to enhance the discriminative power of the learned features. \
    %         ntr-by-1 matrix, ntr is the number of labeled samples. 
    % maLabeled:	Mask for the labeled samples. n-by-1 matrix, maLabeled(i)=true if sample i is labeled, false else.
    [ftAllNew,transMdl] = ftTrans_tca(X_mat, maSrc, Y_mat, ones(size(Y_mat)));
end