% Colourful Image Compression based on SVD decomposition
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;

acc_file_name = 'Accuracy Record ILSVRC2012 QF 1000.xlsx';
sheet=1;
accuracy = [];
size = [];
x = 2:1000;
x = x';
% [num,txt,raw] = xlsread(acc_file_name, sheet); % Read the excel sheet from local

% for i = 2:1:1001
%     for j = 11:5:56
%         Top1_cell = [Top1_cell num(i,j)];
%     end
% end

xlswrite(acc_file_name,x,sheet,'B3');
