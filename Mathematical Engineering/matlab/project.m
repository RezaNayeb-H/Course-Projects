%% Section 3.2 Questions 2, 3, 4
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
knee_image = fftshift(raw_ifft);
knee_image = abs(knee_image);   
% for removing the phase likely created by 
% noise(because the picture should only be real)
imshow(abs(knee_image), []);

%% Section 4.1
%----- last section code for noisy image: -----
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
noisy_image = fftshift(raw_ifft);
noisy_image = abs(noisy_image);   % for removing the phase likely created by noise(because the picture should only be real)
%----- new code: -----
knee_image = imread("kneeMRI.jpg");
% comment one to see the other histogram
%imhist(noisy_image);
%legend('noisy image')
imhist(knee_image);
legend('noisless image')
%% section 4.3 mean filter
%----- noisy image construction -----
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
noisy_image = fftshift(raw_ifft);
noisy_image = abs(noisy_image);   % for removing the phase likely created by noise(because the picture should only be real)
%----- new code: -----
mean_knee = meanflt2(noisy_image, 3);    
imshow(mean_knee, []);
%% section 4.4 median filter
%----- noisy image construction -----
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
noisy_image = fftshift(raw_ifft);
noisy_image = abs(noisy_image);   % for removing the phase likely created by noise(because the picture should only be real)
%----- new code: -----
med_knee = medianflt2(noisy_image, 3);    
imshow(med_knee, []);

%% section 4.5 gaussian filter
%----- noisy image construction -----
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
noisy_image = fftshift(raw_ifft);
noisy_image = abs(noisy_image);   % for removing the phase likely created by noise(because the picture should only be real)
%----- new code: -----
gauss_knee = gaussianflt2(noisy_image, 3, 1);    
imshow(gauss_knee, []);

%% section 4.6 non-local means
%----- noisy image construction -----
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
noisy_image = fftshift(raw_ifft);
noisy_image = abs(noisy_image);   % for removing the phase likely created by noise(because the picture should only be real)
%----- new code: -----
imnlm_knee = imnlmfilt(noisy_image);
imshow(imnlm_knee, []);

%% section  4.7 Evaluation
clc;clear;
%----- noisy image construction -----
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);

raw_ifft = ifft2(dat);
noisy_image = fftshift(raw_ifft);
noisy_image = abs(noisy_image);   % for removing the phase likely created by noise(because the picture should only be real)
%----- new code: -----
knee_image = imread("kneeMRI.jpg");
% here we normalize both vectors to make our algorithms work correctly
noisy_image_norm = noisy_image;
knee_image_norm = double(knee_image) /  256;

mean_knee = meanflt2(noisy_image_norm, 3);
med_knee = medianflt2(noisy_image_norm, 3);
gauss_knee = gaussianflt2(noisy_image_norm, 3, 1);
imnlm_knee = imnlmfilt(noisy_image_norm);

info = ["SNR"; "PSNR"];
noisy = [mSNR(mean_knee, knee_image_norm); mPSNR(noisy_image_norm, knee_image_norm)];
mean = [mSNR(mean_knee, knee_image_norm); mPSNR(mean_knee, knee_image_norm)];
median = [mSNR(med_knee, knee_image_norm); mPSNR(med_knee, knee_image_norm)];
gauss = [mSNR(gauss_knee, knee_image_norm); mPSNR(gauss_knee, knee_image_norm)];
imnlm = [mSNR(imnlm_knee, knee_image_norm); mPSNR(imnlm_knee, knee_image_norm)];
tab = table(info,noisy,mean,median,gauss,imnlm)

%% functions
% mean filter
function mean = meanflt2(data, n)
    N = ones(n) ./ n;
    mean = conv2(data, N, 'same');
end
% median filter
function median = medianflt2(data, n)
    median = medfilt2(data, [n, n]);
end
% gaussian filter
function gfilt = gaussianflt2(data, n, sigma)
    ind = -floor(n/2) : floor(n/2);
    [X Y] = meshgrid(ind, ind);
    
    %// Create Gaussian Mask
    h = exp(-(X.^2 + Y.^2) / (2*sigma*sigma));
    %// Normalize so that total area (sum of all weights) is 1
    h = h / sum(h(:));
    gfilt = conv2(data, h, "same");
end

% SNR Evaluation
function snr = mSNR(I_noisy, K_noisless)
    n = I_noisy.^2;
    num = sum(n, "all");
    d = (I_noisy - K_noisless).^2;
    denum = sum(d, "all");
    snr = 10 * log10(num/denum);
end

% PSNR Evaluation
function psnr = mPSNR(I_noisy, K_noisless)
    [m, n] = size(I_noisy);
    MSE = sum(  (I_noisy - K_noisless).^2  , "all"  ) / (m*n);
    psnr = 10 * log10(    max((I_noisy).^2, [], "all") / MSE);
end


