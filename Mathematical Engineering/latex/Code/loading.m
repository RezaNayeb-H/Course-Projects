% lodaing data from rawkneedata.mat
clc; clear;
mat = load('rawkneedata.mat');
[ans, dat] = deal(mat.ans, mat.dat);