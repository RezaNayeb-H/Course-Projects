% gaussian filter
function gfilt = gaussianflt2(data, n, sigma)
    ind = -floor(n/2) : floor(n/2);
    [X Y] = meshgrid(ind, ind)
    
    %// Create Gaussian Mask
    h = exp(-(X.^2 + Y.^2) / (2*sigma*sigma));
    %// Normalize so that total area (sum of all weights) is 1
    h = h / sum(h(:));
    gfilt = conv2(data, h, "same");
end