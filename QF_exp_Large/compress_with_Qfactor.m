file = 'ILSVRC2012_val_00000520';
file_suffix = '.JPEG';
middle = '-QF-';
I = imread(strcat(file,file_suffix));
% I = imresize(I, 0.5);
% qualityFactor = 0:10:100;
% for i = 0:10:100
%     eval(['imwrite(I,','''Lena/Lena-',num2str(i),'.jpg'',''jpg'',''quality'',',num2str(i),');']);
% end

for i = 10:10:100
    eval(['imwrite(I,','strcat(file,middle,num2str(i),file_suffix)',',''jpg'',''quality'',',num2str(i),');']);
end

for i = 0:10:100
    eval(['imwrite(I,','''ILSVRC2012_val_00000520-QF-',num2str(i),'.JPEG'',''jpg'',''quality'',',num2str(i),');']);
end
