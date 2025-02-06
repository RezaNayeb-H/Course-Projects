% mean filter
function mean = meanflt2(data, n)
    N = ones(n) ./ n;
    mean = conv2(data, N, 'same');
end