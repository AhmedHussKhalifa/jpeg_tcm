% Colourful Image Compression based on SVD decomposition
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;

quality = 100:-10:10;

% file_name = 'lion'; %1
% file_name = 'dog'; %2
% file_name = 'lizzard';%3
% file_name = 'goose';%4
% file_name = 'hawk';%5
% file_name = 'owl';%6
% file_name = 'spoonbill';%7
file_name = 'frog';%8

% file_suffix = '.jpg';
file_suffix = '.JPEG';

acc_file_name = 'Accuracy Record ILSVRC2012 Top 5.xls';
sheet=1;
accuracy = [];
size = [];
for i = 100:-10:10
    file = strcat(file_name,'-QF-', num2str(i),file_suffix);
    s=dir(file);
    size = [size s.bytes];
end

for j = 2:11
    cell_location = strcat('D',num2str(j));
    data=xlsread(acc_file_name,sheet,cell_location);
    
    accuracy = [accuracy data];
end
 

plot(quality, accuracy);
xlabel(' Quality Value in JPEG');
ylabel('Accuracy in %');
% ylim([0 95.5])
set(gca, 'XDir','reverse')
title(strcat(upper(file_name),upper(file_suffix), ' Quality of JPEG vs TOP 1 accuracy'));