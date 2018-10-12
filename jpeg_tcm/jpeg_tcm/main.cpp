//
//  main.cpp
//  TCM
//
//  Created by Hossam Amer on 2018-06-27.
//  Copyright © 2018 Hossam Amer. All rights reserved.
//

#include "tcm.h"
#include <iostream>
#include <math.h>
#include <ctime>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "jpegdecoder.h"

#include "jpegencoder.h"

using namespace std;
using namespace cv;


#define RUN_DECODER_FULL  1
// Flag for whether this run is for multiple picture or not
#define MULTI_PIC         0



int main(int argc, const char * argv[]) {
    // insert code here...
    
    /// JPEG Stuff
    std::string path_to_files = "/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/";
    
    // yes all lena
          std::string filename = path_to_files + "Lena.jpg";
//            std::string filename = path_to_files + "Lena_progressive.jpg";
    //      std::string filename =  path_to_files + "lena_g.jpg";
    //      std::string filename =  path_to_files + "lenaV.jpg";
    //      std::string filename =  path_to_files + "Lena256.jpg";
    //      std::string filename =  path_to_files + "nature.jpeg";
    //      std::string filename =  path_to_files + "pepper.jpg";
    //      std::string filename =  path_to_files + "river.jpg";
    //		std::string filename =  path_to_files + "tree.jpg";
    //		std::string filename =  path_to_files + "whitewoman.jpg";
    //      std::string filename =  path_to_files + "baboon.jpg";
    //      std::string filename =  path_to_files + "Cross.jpeg";
    //      std::string filename =  path_to_files + "testDown.jpeg";
    //      std::string filename =  path_to_files + "Hossam_Amer.jpg";
    //      std::string filename =  path_to_files + "Yang.jpg";
    //    std::string filename = path_to_files + "goose_org.jpg";
    //		std::string filename = path_to_files + "dog.JPEG";
    //		std::string filename = path_to_files + "owl.JPEG";
    //		std::string filename = path_to_files + "ILSVRC2012_val_00000878.JPEG";
    //		std::string filename = path_to_files + "lion.jpg";
    
    
    // No:
    //        std::string filename =  path_to_files + "test.jpeg";
//        std::string filename =  path_to_files + "mountain.jpg"; // 444 chroma subsampling
//        std::string filename =  path_to_files + "mountainview1.jpg";// 444 chroma subsampling

    
    // Start time:
    clock_t begin = clock();
    
    
    jpeg_decoder test(filename);
    
    
    //    std::string output_fileName = path_to_files + "myLena.bmp";
    std::string output_fileName = path_to_files + "Hossam_Amer.bmp";
//    std::string output_fileName = path_to_files + "En-hui Yang.bmp";
    
    
#if RUN_DECODER_FULL
    // Remove display
     int output = test.convert_jpg_to_bmp(output_fileName);
    
    
    // End time:
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC);
    cout << "Elapsed Time is: " << elapsed_secs << " seconds." << endl;
    
    // Display Image in OpenCV:
//    if(output) {
//        std::size_t found = output_fileName.find_last_of("/\\");
//        std::string name_file_only = output_fileName.substr(found+1);
//        Mat img = imread(output_fileName, CV_LOAD_IMAGE_COLOR);
//        imshow(name_file_only, img);
//        waitKey(0);
//    }
    
    // Remove display:
    test.display_jpg_yuv("Gray Scale Output", 0);
    
#endif
    
//    test.write_yuv_from_jpg_in_csv(path_to_files + "howy.csv");
//    string pF = path_to_files + "Fankoosh.yuv";
//    test.write_yuv_from_jpg_in_yuv(pF);

    // Encoding:
    std::string encoded_filename = path_to_files + "Lena_encoded.jpg";
    jpeg_encoder enc(&test, encoded_filename);
    enc.savePicture();
    
//
//    FILE* pFile;
//    string pF = path_to_files + "howy.yuv";
//    const static char* pFileName = pF.c_str();
//    pFile = fopen (pFileName, "wb");
//    
//        for (int y = 0; y < 512; y++ )
//        {
//            for (int x = 0; x < 512; x++ )
//            {
//                uint_8 uc = ptr[x][y];
////                      uint_8    uc = 128;
//                fwrite( &uc, sizeof(uint_8), 1, pFile );
//            }
//        }
//    
//    fclose(pFile);
//    
//    
//    ofstream myfile;
//    myfile.open (path_to_files + "howy.csv");
//    for (int y = 0; y < 512; ++y ) {
//        for(int x = 0 ; x < 512; ++x ) {
//            uint_8 uc = ptr[x][y];
//            myfile << static_cast<int>(uc) << ",";
//        }
//        myfile << "\n";
//    }
//    
//    
//    
//    myfile.close();
//    
//    
//////    cv::Mat img(image_rows,image_cols,image_type,image_uchar,cv::Mat::AUTO_STEP);
//    cv::Mat img2(512,512,CV_8U,ptr2,cv::Mat::AUTO_STEP);
//    imshow("Gray Scale Output", img2);
//    waitKey(0);
    
    
    
    return 0;
}
