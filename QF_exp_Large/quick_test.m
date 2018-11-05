% for i = 1:1000  
%     
%      B3(i)
%     if B3(i) >30
%              display('Hello0!');
% 
%         D(i) = 0;
%     else 
%              display('Hello1!');
% 
%         D(i) = 1;
%     end
% end

% A1 = imread('ILSVRC2012_val_00000004-QF-0.JPEG');
% A2 = imread('ILSVRC2012_val_00000004-QF-10.JPEG');
% A3 = imread('ILSVRC2012_val_00000004-QF-20.JPEG');
% A4 = imread('ILSVRC2012_val_00000004-QF-30.JPEG');
% A5 = imread('ILSVRC2012_val_00000004-QF-40.JPEG');
% A6 = imread('ILSVRC2012_val_00000004-QF-50.JPEG');
% A7 = imread('ILSVRC2012_val_00000004-QF-60.JPEG');
% A8 = imread('ILSVRC2012_val_00000004-QF-70.JPEG');
% A9 = imread('ILSVRC2012_val_00000004-QF-80.JPEG');
% A10 = imread('ILSVRC2012_val_00000004-QF-90.JPEG');
% A11 = imread('ILSVRC2012_val_00000004-QF-100.JPEG');

% A11 = imread('C:\Users\y77jiang\Downloads\ILSVRC2012\set\ILSVRC2012_val_00000429.JPEG');
A11 = imread('C:\Users\y77jiang\Downloads\ILSVRC2012\set\ILSVRC2012_val_00000001.JPEG');

% A12 = (A4+A5+A6+A7+A8+A9+A10+A11); %+A3+

A12 = A11;
A12 = rangefilt(A11);
imshow(uint8(A12));