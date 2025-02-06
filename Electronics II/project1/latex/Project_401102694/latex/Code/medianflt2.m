% median filter
function median = medianflt2(data, n)
    median = medfilt2(data, [n, n]);
end