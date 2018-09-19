////// zigZagArray variable will hold data after zigZag transform
//////// image variable holds data for one of three components (YCbCr)
//void::jpeg_encoder::perform_fdct(uint_8 ** image, vector<int> &zigZagArray, int quantizationTable [8][8], int currentComponent) {
//    
//    vector<vector<double> >block8x8(8,vector<double>(8));
//    double previousDCCoefficient = 0; //In this function DCPM is performed so DC coeficient is generated with formula Diff=DCi-DCi-1
//    double prev_previousDC = 0;// previous previous
//    
//    int image_width  = image_to_export_width_dct;
//    int image_height = image_to_export_height_dct;
//    
//    // How many Y's in the horizontal and vertical direction (2x2 is the usual case)
//    int HFactor = jpegDecoder->components[currentComponent].HFactor, VFactor = jpegDecoder->components[currentComponent].VFactor;
//    
//    // These Y's should be scaled/repeated how many times horizonatally and vertically
//    int HScale = jpegDecoder->components[currentComponent].HScale,   VScale = jpegDecoder->components[currentComponent].VScale;
//    
//    
//    int comp_height = ceil(image_height / (double) VScale);
//    int comp_width  = ceil(image_width  / (double) HScale);
//    int width = ceil(image_to_export_width / (double) VScale), height = ceil(image_to_export_height / (double) VScale);
//    
//    int currentX = 0;
//    int currentY = 0;
//    int currentBlockHFactor = 0;
//    int currentBlockVFactor = 0;
//    
//    // loop on the entire picture
//    for (uint i = 0; i < ceil(comp_height/8.0); ++i) {
//        for (uint j = 0; j < ceil(comp_width / 8.0); ++j) {
//            // fetch the block
//#if DEBUGLEVEL > 20
//            cout << "Top Left: " << currentX << ", " << currentY << ", Bottom Right: " << currentX + 8 << ", " << currentY + 8 << endl;
//#endif
//            uint y;
//            for(y = 0; y < 8; ++y) {
//                
//                if (currentX >= width) break;
//                if (currentY + y >= height) break;
//                
//                int picture_y = currentY + y;
//                
//                uint x;
//                for(x = 0; x < 8; ++x) {
//					
//                    if (currentX + x * HScale >= width) break;
//                    int realx = currentX + x;
//                    
//                    int u = y % 8;
//                    int v = x % 8;
//					//cout << "picture_y = " << picture_y << "    realx = " << realx << endl;
//
//					//if ((i == ceil(comp_height / 8.0) - 1 && padded_height) || (j == ceil(comp_width / 8.0) - 1 && padded_width)) {
//					//	int in_block[8][8] = { 0 };
//					//	if (currentComponent == COMPONENT_Y)
//					//		in_block[u][v] = jpegDecoder->m_YPicture_buffer_enc[y + u] [x + v];
//					//	else if(currentComponent == COMPONENT_Cb)
//					//		in_block[u][v] = jpegDecoder->m_CbPicture_buffer_enc[y + u] [x + v];
//					//	else
//					//		in_block[u][v] = jpegDecoder->m_CrPicture_buffer_enc[y + u] [x + v];
//					//	DCT2((int**)in_block, block8x8);
//					//}
//					//else {
//						if (currentComponent == COMPONENT_Y)
//							block8x8[u][v] = jpegDecoder->tCoeff_Y[picture_y][realx];
//						else if (currentComponent == COMPONENT_Cb)
//							block8x8[u][v] = jpegDecoder->tCoeff_Cb[picture_y][realx];
//						else
//							block8x8[u][v] = jpegDecoder->tCoeff_Cr[picture_y][realx];
//					//}
//					
//
//					/*cout << "input" << endl;
//					for (int m = 0; m < 8; m++) {
//						for (int n = 0; n < 8; n++) {
//							cout << setw(8) << in_block[m][n] << " \t";
//						}
//						cout << endl;
//					}*/
//
//                }
//            }
//
//			// DCT for the block
//			//perform_dct(block8x8, in_block);
//			//DCT2((int**)in_block, block8x8);
//                       
//            // Adjust the indices:
//            HScale = 1; // note: you will store the dct coefficients without any scaling
//            VScale = 1; // note: algorithm below works, but you have to set the scales into 1x1
//            currentX += 8 * HScale;
//            currentBlockHFactor++;
//            // you made a line of blocks
//            if(currentBlockHFactor >= HFactor) {
//                
//                // restore the current X to its initial position and reset the counters
//                currentX -= 8 * HScale * HFactor;
//                currentBlockHFactor = 0;
//                
//                // go to next line
//                currentY += 8 * VScale;
//                currentBlockVFactor++;
//                
//                // you made a column of blocks
//                if (currentBlockVFactor >= VFactor) {
//                    
//                    // restore the current Y to its initial position and reset the counters
//                    currentY -= 8  * VScale * VFactor;
//                    currentBlockVFactor = 0;
//                    currentX += 8 * HScale * HFactor;
//                    if (currentX >= width) {
//                        currentX = 0;
//                        currentY += 8  * VScale * VFactor;
//                    } // end if (currentBlockVFactor_dct[comp] >= VFactor)
//                    
//                } // end  if (currentBlockVFactor_dct[comp] >= VFactor)
//            } // end if(currentBlockHFactor_dct[comp] >= HFactor)
//
//            
//            
//            // DCPM for the DC element of the compononet (including quantization)
//            for (int u = 0; u < 8; ++u) {
//                for (int v = 0; v < 8; ++v) {
//                    block8x8[u][v] = block8x8[u][v] / quantizationTable[u][v];
//                    if(!u && !v) {
//                        
//                        // Store the DC coefficient
//                        prev_previousDC = block8x8[u][v];
//
//#if DEBUGLEVEL > 20
//                        cout << "Current DC coefficient: " << block8x8[u][v] << ", Prev: " << previousDCCoefficient << endl;
//#endif
//                        
//                        // TODO: double check this (i think it should be OK)
//                        block8x8[u][v] = block8x8[u][v] - previousDCCoefficient;
//                        
//                        // quantization
//                        // TODO: check quantization table -- possible bug
//                        previousDCCoefficient = prev_previousDC; // set the previous DC
////                        previousDCCoefficient = block8x8[u][v]; // set the previous DC
//                        
//                        
//                    }
//                }
//            }
//            
//            // Perform ZigZag coding, and array is ready for Huffman coding
//            ZigZagCoding(block8x8, zigZagArray);
//            
//             // TODO: remove FANKOOSH
//            static int fankoosh = 0;
//            fankoosh++;
//            if(currentComponent == COMPONENT_Y ) {
//                ofstream myfile;
//                std::string path_to_files = "C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/JPEG/JPEG/";
//                std::string output_csv_name = path_to_files + "Lena_Y_enc.csv";
//                myfile.open (output_csv_name, std::ofstream::out | std::ofstream::app);
//                
//                std::stringstream oss;
//                std::size_t found = output_csv_name.find_last_of(".");
//                std::string path_with_name = output_csv_name.substr(0, found);
//                found = output_csv_name.find_last_of("/\\");
//                std::string name_file_only = path_with_name.substr(found+1);
//                
//                int k = 0;
//                for(int i = 0; i < 8; ++i) {
//                    for(int j = 0; j < 8; ++j) {
//                        //                        myfile << block8x8[i][j] << ",";
//                        int val = zigZagArray.at(zigZagArray.size() - 64 + k++);
//                        myfile << val << ",";
//                        //                        myfile << block8x8[i][j] << ",";
//                    }
//                    
//                    //                    if( (i + 1) < 8){
//                    myfile << "\n";
//                    //                    }
//                }
//                
//                //        myfile << "\n------------------\n";
//                myfile.close();
//                
//                
//            }
////            else
////            {
////                exit(0);
////            }
//            // fankoosh
////            static int beb = 0;
////            if(beb++ == 4)
////                exit(0);
//            
//        }
//    }
//} // end perform_fdct
