%% PSD example
clc; clear all; close all;
tt = edfread("chb01_16.edf");
data = tt2Mat(tt);

pwelch(data)

%% shannon entropy
clc; clear all; close all;
mc02 = tt2Mat(edfread("chb01_02.edf"));
shannonE = shannonEpoch16(getDataBeforeTime(mc02,600,700,256), 16, 256);
plot(shannonE);
title('Shannon Entropy for 16 second Epochs on differnet Channels');
%% Loading data and setting 
clc; clear all; close all;
mc02 = tt2Mat(edfread("chb01_02.edf"));
mc03 = tt2Mat(edfread("chb01_03.edf"));
mc04 = tt2Mat(edfread("chb01_04.edf"));
mc15 = tt2Mat(edfread("chb01_15.edf"));
mc16 = tt2Mat(edfread("chb01_16.edf"));
mc18 = tt2Mat(edfread("chb01_18.edf"));
mc26 = tt2Mat(edfread("chb01_26.edf"));

%% Feature extraction Section
[A1,info] = getFeature(getDataBeforeTime(mc02,600,700,256), 16, 256);
[A2,~] = getFeature(getDataBeforeTime(mc02,600,1400,256), 16, 256);
[A3,~] = getFeature(getDataBeforeTime(mc02,600,2100,256), 16, 256);
[A4,~] = getFeature(getDataBeforeTime(mc02,600,2600,256), 16, 256);
[A5,~] = getFeature(getDataBeforeTime(mc02,600,3100,256), 16, 256);
[A6,~] = getFeature(getDataBeforeTime(mc02,600,3600,256), 16, 256);
[B1,~] = getFeature(getDataBeforeTime(mc03,600,2400,256), 16, 256);
[B2,~] = getFeature(getDataBeforeTime(mc18,600,800,256), 16, 256);
[C1,~] = getFeature(getDataBeforeTime(mc03,600,3100,256), 16, 256);
[C2,~] = getFeature(getDataBeforeTime(mc04,600,1500,256), 16, 256);
[C3,~] = getFeature(getDataBeforeTime(mc15,600,1800,256), 16, 256);
[C4,~] = getFeature(getDataBeforeTime(mc16,600,1100,256), 16, 256);
[C5,~] = getFeature(getDataBeforeTime(mc18,600,1850,256), 16, 256);
[C6,~] = getFeature(getDataBeforeTime(mc26,600,2100,256), 16, 256);

allFeatures = [ A1; A2; A3; A4; A5; A6; B1; B2; C1; C2; C3; C4; C5; C6];

[~,p] = ttest2(allFeatures(1:8,:),allFeatures(9:end,:));
pvalue = 0.005;
features = [];
finfo = [];
for i = 1:length(p)
    if p(i) < pvalue
        features = [features allFeatures(:,i)];
        finfo = [finfo info(i)];
    end
end

label = [0; 0; 0; 0; 0; 0; 0; 0; 1; 1; 1; 1; 1; 1];
trainLabel = [label(1:5); label(7); label(9:12)];
trainData = [features(1:5,:); features(7,:); features(9:12,:)];
testData = [features(6,:); features(8,:); features(13:14,:)];
%% SVM
clc;
SVMclassifier = fitcsvm(trainData,trainLabel);
[labelSVM,scoreSVM] = predict(SVMclassifier, testData);

%% KNN
clc;
KNNmodel = fitcknn(trainData,trainLabel);
[labelKNN,scoreKNN] = predict(KNNmodel, testData);


%% Performance
clc;
[knnTruePer, knnFalsePer] = leaveOneOutKnn(features, label);
[svmTruePer, svmFalsePer] = leaveOneOutSVM(features, label);

fprintf("KNN True Positive rate is %.2f\n", knnTruePer)
fprintf("KNN True Negative rate is %.2f\n", knnFalsePer)
fprintf("SVM True Positive rate is %.2f\n", svmTruePer)
fprintf("SVM True Negative rate is %.2f\n", svmFalsePer)

%% Fuctions
% the following function converts a timeTable to Matrix
% each column represents one EEG channel
% each row represents one sample in time
function data = tt2Mat(tt)
    cells = tt{:,:};
    data = cell2mat(cells);
end

function signal = getDataBeforeTime(input_signal,duration, end_t, sampling)
    end_point = end_t*sampling;
    start_point = end_point - duration*sampling;
    signal = input_signal(start_point:end_point-1,:);
end

function [f,info] = getFeature(signal, lenSec, fs)
    info = [];
    f = [];
    n = fs*lenSec;
    for i = 0:floor(length(signal)/n)-1
        e = signal(i*n+1:(i+1)*n,:);
        normPSD = normalize(pwelch(e));
        shannon = transpose(wentropy(transpose(normPSD)));
        mu = mean(e);
        sigma = std(e);
        minVal = min(e);
        maxVal = max(e);
        f = [f shannon mu sigma maxVal minVal];
        for j = 1:23
            info = [info "ent" + string(i) + "-"+ string(j)];
        end

        for j = 1:23
            info = [info "mean" + string(i) + "-" + string(j)];
        end

        for j = 1:23
            info = [info "std" + string(i) + "-" + string(j)];
        end

        for j = 1:23
            info = [info "max" + string(i) + "-" + string(j)];
        end

        for j = 1:23
            info = [info "min" + string(i) + "-" + string(j)];
        end

    end

    if mod(length(signal),n) ~= 0
        e = signal(i*n+1:end,:);
        normPSD = normalize(pwelch(e));
        shannon = wentropy(normPSD')';
        mu = mean(e);
        sigma = std(e);
        minVal = min(e);
        maxVal = max(e);
        f = [f shannon mu sigma maxVal minVal];
        for j = 1:23
            info = [info "ent" + string(i) + "-"+ string(j)];
        end

        for j = 1:23
            info = [info "mean" + string(i) + "-" + string(j)];
        end

        for j = 1:23
            info = [info "std" + string(i) + "-" + string(j)];
        end

        for j = 1:23
            info = [info "max" + string(i) + "-" + string(j)];
        end

        for j = 1:23
            info = [info "min" + string(i) + "-" + string(j)];
        end
    end
end

% leave one out for knn and svm
function [percentileTrue, percentileFalse] = leaveOneOutKnn(features, label)
    knnTrueCorrect = 0;
    knnFalseCorrect = 0;
    trueMax = 0;
    falseMax = 0;

    for i = 1:size(features,1)
        f = features;
        f(i,:) = [];
        l = label;
        l(i) = [];
        knn = fitcknn(f,l);
        [knnp,~] = predict(knn, features(i,:));
        if label(i) == 1
            trueMax = trueMax + 1;
        end

        if label(i) == 0
            falseMax = falseMax + 1;
        end

        if (label(i)==1) && (knnp == 1)
            knnTrueCorrect = knnTrueCorrect + 1;
        end

        if (label(i)==0) && (knnp == 0)
            knnFalseCorrect = knnFalseCorrect + 1;
        end


    end
    percentileTrue = knnTrueCorrect*100/trueMax;
    percentileFalse = knnFalseCorrect*100/falseMax;
end

function [percentileTrue, percentileFalse] = leaveOneOutSVM(features, label)
    svmTrueCorrect = 0;
    svmFalseCorrect = 0;
    trueMax = 0;
    falseMax = 0;
    svmcorrect = 0;
    for i = 1:size(features,1)
        f = features;
        f(i,:) = [];
        l = label;
        l(i) = [];
        svm = fitcsvm(f,l);
        [svmp,~] = predict(svm, features(i,:));
        if label(i) == 1
            trueMax = trueMax + 1;
        end

        if label(i) == 0
            falseMax = falseMax + 1;
        end

        if (label(i)==1) && (svmp == 1)
            svmTrueCorrect = svmTrueCorrect + 1;
        end

        if (label(i)==0) && (svmp == 0)
            svmFalseCorrect = svmFalseCorrect + 1;
        end


    end
    percentileTrue = svmTrueCorrect*100/trueMax;
    percentileFalse = svmFalseCorrect*100/falseMax;
end



function y = shannonEpoch16(signal, lenSec, fs)
    y = [];
    n = fs*lenSec;
    for i = 0:floor(length(signal)/n)-1
        e = signal(i*n+1:(i+1)*n,:);
        npsd = normalize(pwelch(e));
        shannon = transpose(wentropy(transpose(npsd)));
        y = [y; shannon];
    end

    if mod(length(signal),n) ~= 0
        e = signal(i*n+1:end,:);
        npsd = normalize(pwelch(e));
        shannon = wentropy(npsd')';
        y = [y; shannon];
    end
end