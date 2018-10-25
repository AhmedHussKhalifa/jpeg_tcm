% Colourful Image Compression based on SVD decomposition
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;

quality = 100:-10:0;

acc_file_name = 'Accuracy Record ILSVRC2012 QF.xls';
sheet=1;
accuracy = [];
size = [];
[num,txt,raw] = xlsread(acc_file_name, sheet); % Read the excel sheet from local
%% Groud Truth Accuracy Depends on Highest First one accuracy
Top1_cell = [];
MAX_selection_QF = [];
figure;

k = 1;
for i = 2:1001
    for j = 11:5:11
        Top1_cell = [Top1_cell num(i,j)];
        MAX_selection_QF = [MAX_selection_QF num(i,60)];
        plot(Top1_cell,MAX_selection_QF,'o');
        hold on;
        
        val_100 = Top1_cell;
        k = k + 1;
    end

end

k = 1;

figure;
Top1_cell= [];
MAX_selection_QF = [];

for i = 2:1001
    for j = 16:5:16
        Top1_cell = [Top1_cell num(i,j)];
        MAX_selection_QF = [MAX_selection_QF num(i,60)];
        plot(Top1_cell,MAX_selection_QF,'o');
        hold on;
        

    end

end
val_90 = Top1_cell;
qf_vector = MAX_selection_QF;

figure;
matrix = [val_100;val_90;qf_vector];
mesh(val_100, val_90, qf_vector);