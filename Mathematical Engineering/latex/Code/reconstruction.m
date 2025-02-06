% picture reconstruction using ifft2
raw_ifft = ifft2(dat);
knee_image = fftshift(raw_ifft);
knee_image = abs(knee_image);   
% for removing the phase likely created by 
% noise(because the picture should only be real)
imshow(abs(knee_image), []);