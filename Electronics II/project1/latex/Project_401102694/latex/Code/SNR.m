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
