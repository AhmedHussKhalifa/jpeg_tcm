% Colourful Image Compression based on SVD decomposition
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
clc;

quality = 100:-10:0;

acc_file_name = 'Accuracy Record ILSVRC2012 QF.xls';
sheet=1;
accuracy = [];
size = [];
% for i = 100:-10:10
%     file = strcat(file_name,'-QF-', num2str(i),file_suffix);
%     s=dir(file);
%     size = [size s.bytes];
% end

% for j = 2:12
%     cell_location = strcat('D',num2str(j));
%     data=xlsread(acc_file_name,sheet,cell_location);    
%     accuracy = [accuracy data];
% end
[num,txt,raw] = xlsread(acc_file_name, sheet); % Read the excel sheet from local
%% MAX QF Method
% maxGT_cell = [];
% GT_acc = [];
% QF = [];
% rank = []; 
% for i = 2:1:1001
%     for j = 9:5:54
%         maxGT_cell = [maxGT_cell num(i,j)];
%     end
%     [Max_GT Max_GT_index] = max(maxGT_cell);
%     GT_acc = [GT_acc; Max_GT];
%     QF = [QF;110-10*Max_GT_index];
%     rank = [rank; num(i,8+5*(Max_GT_index-1))];
%     maxGT_cell = [];
% end
%  
% GT_avg = mean(GT_acc);
% QF_avg = mean(QF);
% 
% xlswrite(acc_file_name,rank,sheet,'BF2');
% xlswrite(acc_file_name,GT_acc,sheet,'BG2');
% xlswrite(acc_file_name,QF,sheet,'BH2');

%% Groud Truth Accuracy Depends on Highest First one accuracy
Top1_cell = [];
GT_acc = [];
QF = [];
rank = []; 
First_1 = [];
for i = 2:1:1001
    for j = 11:5:56
        Top1_cell = [Top1_cell num(i,j)];
    end
    [Max_Top1 Max_Top1_index] = max(Top1_cell);
    GT_acc = [GT_acc; num(i,9+5*(Max_Top1_index-1))];
    QF = [QF;110-10*Max_Top1_index];
    rank = [rank; num(i,8+5*(Max_Top1_index-1))];
    First_1 = [First_1; Max_Top1];
    Top1_cell = [];
end
 
GT_avg = mean(GT_acc);
QF_avg = mean(QF);

xlswrite(acc_file_name,rank,sheet,'BL2');
xlswrite(acc_file_name,GT_acc,sheet,'BM2');
xlswrite(acc_file_name,QF,sheet,'BN2');
% xlswrite(acc_file_name,First_1,sheet,'BO2');

%% Plotting Part
% plot(quality, accuracy, '-*');
% a = quality'; b = strcat(num2str(round(100*accuracy, 2)'),'%'); c = cellstr(b);
% dy = -0.02; % displacement so the text does not overlay the data points
% text(quality, accuracy+dy, c);
% hold on;
% plot([quality(1) quality(end)],[org_acc org_acc],'LineWidth',1.2);
% legend('QF',strcat('Original = ',num2str(round(100*org_acc, 2)),'%'));
% xlabel(' Quality Value in JPEG');
% ylabel('Accuracy in %');
% % ylim([0 95.5])
% set(gca, 'XDir','reverse')
% title(strcat(upper(file_name),upper(file_suffix), ' Quality of JPEG vs TOP 1 accuracy'));