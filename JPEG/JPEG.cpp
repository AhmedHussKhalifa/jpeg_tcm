// JPEG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//
//  main.cpp
//  TCM
//
//  Created by Hossam Amer on 2018-06-27.
//  Author: Hossam Amer & Yanbing Jiang, University of Waterloo
//  Copyright © 2018 All rights reserved.
//

#include <iostream>
#include <io.h> 
#include <math.h>
#include <ctime>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "jpegimage.h"

#include "jpegdecoder.h"

#include "jpegencoder.h"

#include "jpegencoderMultipleQF.h"


using namespace std;
using namespace cv;


#define RUN_DECODER_FULL  0
// Flag for whether this run is for multiple picture or not
#define MULTI_PIC         1


void getAllFiles(string path_in, string path_out, vector<string>& in_files, vector<string>& out_files, string format)
{
	long  hFile = 0;
	struct _finddata_t fileinfo; //document info in io.h
	string p;
	if ((hFile = _findfirst(p.assign(path_in).append("\\*" + format).c_str(), &fileinfo)) != -1) //if file exists
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) //remove '.' and '..'
			{
				in_files.push_back(p.assign(path_in).append("\\").append(fileinfo.name)); // save corresponding file name
				out_files.push_back(p.assign(path_out).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


int main(int argc, const char * argv[]) {

#if !MULTI_PIC
	// JPEG Stuff
	std::string path_to_files = "C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/jpeg_tcm/dataset/";
	//std::string path_to_files = "C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/jpeg_tcm/JPEG/";
	//std::string path_to_files = "C:/Users/y77jiang/Downloads/ILSVRC2012/set/";

	 std::string enc_path_to_files = "C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/jpeg_tcm/dataset/";
	  //std::string enc_path_to_files  = "C:/Users/y77jiang/Downloads/ILSVRC2012/set/";


	// yes all lena
    //       std::string filename = path_to_files + "Lena.jpg";

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
	//		std::string filename = path_to_files + "goose.jpg";
	//		std::string filename = path_to_files + "dog.JPEG";
	//		std::string filename = path_to_files + "owl.JPEG";
	//		std::string filename = path_to_files + "phone.JPEG";
	//		std::string filename = path_to_files + "mouse.JPEG";
			std::string filename = path_to_files + "godonla.JPEG";
	//		std::string filename = path_to_files + "spoonbill.JPEG";
	//		std::string filename = path_to_files + "lizzard.JPEG";
	//		std::string filename = path_to_files + "lizzard-20.jpg";
	//		std::string filename = path_to_files + "frog.JPEG";
	//		std::string filename = path_to_files + "hawk.JPEG";
	//		std::string filename = path_to_files + "lion.jpg";
	
		

	// No:
	 //     std::string filename =  path_to_files + "test.jpeg";
	//      std::string filename =  path_to_files + "mountain.jpg"; // 444 chroma subsampling
	//      std::string filename =  path_to_files + "mountainview1.jpg";// 444 chroma subsampling


	// Start time:
	clock_t begin = clock();
	jpeg_decoder test(filename);

	std::string output_fileName = path_to_files + "myLena.bmp";
	//std::string output_fileName = path_to_files + "Hossam_Amer.bmp";

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
	test.display_jpg_yuv("Gray Scale Output", COMPONENT_Y);
#else
		clock_t end;
		double elapsed_secs;
#endif
	//    test.write_yuv_from_jpg_in_csv(path_to_files + "howy.csv");
	// string pF = path_to_files + "Fankoosh.yuv";
	// test.write_yuv_from_jpg_in_yuv(pF);

	// Encoding:
	//std::string encoded_filename = path_to_files + "Lena_encoded.jpg";
	// std::string encoded_filename = path_to_files + "HS_encoded.jpg";
	begin = clock();

	if (QFACTOR >= 1) {
		// Quality factor experiment:
		////////////////////////////////////////////////////////
		std::string encoded_filename = filename;

		////// String Processing -- Get the file Name
		size_t found = encoded_filename.find_last_of("/\\");
		std::string filename_first_token = encoded_filename.substr(found + 1);
		found = filename_first_token.find_first_of(".");
		std::string filename_second_token = filename_first_token.substr(0, found);


		// for each quality factor
		int end_quality_factor = 100;
		for (int quality_factor = 0; quality_factor <= end_quality_factor; quality_factor += 10)
		{
			// Encoded output name
			std::string enc_path_to_files = "C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/QF_exp/";
			encoded_filename = enc_path_to_files + filename_second_token + "-QF-" + to_string(quality_factor) + filename_first_token.substr(found);

			cout << "encoded Output: " << encoded_filename << endl;
			jpeg_encoder enc(&test, encoded_filename, quality_factor);
			enc.savePicture();
		}
	}
	else {
		std::string encoded_filename = enc_path_to_files + "AB.jpg";
		// std::string encoded_filename = path_to_files + "Fast.jpg";
		// std::string encoded_filename = path_to_files + "PeaceN.jpg";
		jpeg_encoder enc(&test, encoded_filename);
		enc.savePicture();
		end = clock();
		elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC);
		cout << "Elapsed Time is: " << elapsed_secs << " seconds." << endl;
		cout << "Pausing at the end" << endl;
		getchar();
		return 0;
	}
	
#else

	// string filePath_in = "C:\\Users\\y77jiang\\OneDrive - University of Waterloo\\5e. TCM-Inception C++\\jpeg_tcm\\dataset\\Multi-pic-input";   //input  folder
	// string filePath_out = "C:\\Users\\y77jiang\\OneDrive - University of Waterloo\\5e. TCM-Inception C++\\jpeg_tcm\\dataset\\Multi-pic-output"; //output folder
	string filePath_in = "C:\\Users\\y77jiang\\Downloads\\ILSVRC2012\\lion";   //input  folder
	string filePath_out = "C:\\Users\\y77jiang\\Downloads\\ILSVRC2012\\output\\"; //output folder

	// Folder files information
	vector<string> in_files;
	vector<string> out_files;
	string format = "";// search file format
	getAllFiles(filePath_in, filePath_out, in_files, out_files, format);
	int size = in_files.size();

	if (QFACTOR >= 1) {

		// Quality factor experiment:
		////////////////////////////////////////////////////////
		for (int i = 0; i < size; i++)
		{
			jpeg_decoder test(in_files[i]);
			std::string encoded_filename = in_files[i];

			////// String Processing -- Get the file Name
			size_t found = encoded_filename.find_last_of("/\\");
			std::string filename_first_token = encoded_filename.substr(found + 1);
			found = filename_first_token.find_first_of(".");
			std::string filename_second_token = filename_first_token.substr(0, found);


			//// for each quality factor
			//int end_quality_factor = 100;
			//for (int quality_factor = 0; quality_factor <= end_quality_factor; quality_factor += 10)
			//{
			//	// Encoded output name
			//	std::string enc_path_to_files = filePath_out;
			//	encoded_filename = enc_path_to_files + "\\" + filename_second_token + "-QF-" + to_string(quality_factor) + filename_first_token.substr(found);

			//	cout << "encoded Output: " << encoded_filename << endl;
			//	jpeg_encoder enc(&test, encoded_filename, quality_factor);
			//	enc.savePicture();
			//}

			runEncoderWithMultipleQF(in_files[i], filePath_out);
			getchar();
		}
	}
	else {
		for (int i = 0; i < size; i++)
		{
			jpeg_decoder test(in_files[i]);
			string encoded_filename = out_files[i];
			cout << "Processing Picture: #" << i << endl;
			jpeg_encoder enc(&test, encoded_filename);
			enc.savePicture();
			cout << "Successfully process: " << in_files[i] << endl;
		}
		getchar();
		return 0;
	}
#endif
}


