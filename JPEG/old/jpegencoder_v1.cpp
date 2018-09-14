//
//  jpegencoder.cpp
//  TCM
//
//  Created by Hossam Amer on 2018-08-16
//  Copyright © 2018 Hossam Amer. All rights reserved.
//
#include "stdafx.h"
#include "jpegencoder.h"
#include <iostream>

const double pi=3.1415926535897932384626433832795;



jpeg_encoder::jpeg_encoder(const std::string filename) : quantization_table_write_process_luminance(true, true), quantization_table_write_process_chrominance(true, false) {
    
    image_to_export_filename = filename;
}

jpeg_encoder::jpeg_encoder(jpeg_decoder* processed_jpeg_decoder, std::string output_filename) : quantization_table_write_process_luminance(true, true), quantization_table_write_process_chrominance(true, false) {
    
    // set the output file name
    image_to_export_filename = output_filename;
    
    // TODO: that's indeed not safe -- you need to create copy constructor or make it unique_ptr
    jpegDecoder = processed_jpeg_decoder;
    
    // TODO: image width and height must be multiples of 8. If they are not, then we have to set them up
    
    
    // set image with and height:
    image_to_export_width  = processed_jpeg_decoder->get_image_width();
	image_to_export_height = processed_jpeg_decoder->get_image_height();
	
	// Set the upscaled image width and height parameters
	image_to_export_width_dct  = processed_jpeg_decoder->upscale_width;
	image_to_export_height_dct = processed_jpeg_decoder->upscale_height;


	// boolean that test whether we padded the input picture 
	if (image_to_export_width_dct != image_to_export_width)  padded_width = true;
	if (image_to_export_height != image_to_export_height_dct)  padded_height = true;

	// TODO: remove thiss
	total_block = image_to_export_width_dct * image_to_export_height_dct / 64;
	total_block_C = ceil(sqrt(total_block) / 2.0) * ceil(sqrt(total_block) / 2.0);
    
    // Set the counts to 0
    count_block_Y = count_block_Cb = count_block_Cr = 0;
    // queue reading stuff
    g_nbits_in_reservoir = 0;
    // the queue of bits
    m_bit_buffer = 0;
    // queue reading stuff
    m_bits_in = 0;
}

void jpeg_encoder::copy_qTables(){
    
    int comp_size = static_cast<int>(jpegDecoder->components.size());
    
    // Copy the quantization tables
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            
            QuantizationTable * from = jpegDecoder->components[COMPONENT_Y].componentQuantizationTable;
            quantization_table_write_process_luminance.quantizationTableData[i][j] = from->quantizationTableData[i][j];
            
            // TODO: check if you only the QTable for Cb and not Cr
            if(comp_size > 1) {
                from = jpegDecoder->components[COMPONENT_Cb].componentQuantizationTable;
                quantization_table_write_process_chrominance.quantizationTableData[i][j] = from->quantizationTableData[i][j];
            }
            
        } // end inner
    } // end outer
    
} // end copy_qTables


//// zigZagArray variable will hold data after zigZag transform
////// image variable holds data for one of three components (YCbCr)
void::jpeg_encoder::perform_fdct(uint_8 ** image, vector<int> &zigZagArray, int quantizationTable [8][8], int currentComponent) {
    
    vector<vector<double> >block8x8(8,vector<double>(8));
    double previousDCCoefficient = 0; //In this function DCPM is performed so DC coeficient is generated with formula Diff=DCi-DCi-1
    double prev_previousDC = 0;// previous previous
      
    // How many Y's in the horizontal and vertical direction (2x2 is the usual case)
    int HFactor = jpegDecoder->components[currentComponent].HFactor, VFactor = jpegDecoder->components[currentComponent].VFactor;
    
    // These Y's should be scaled/repeated how many times horizonatally and vertically
    int HScale = jpegDecoder->components[currentComponent].HScale,   VScale = jpegDecoder->components[currentComponent].VScale;
    
    // Note: use the upscaled width and height to store the DCT coefficientss
    int comp_height = ceil(1.0* image_to_export_height_dct / VScale);
    int comp_width  = ceil(1.0* image_to_export_width_dct / HScale);
    int width = comp_width, height = comp_height;

	// Initialize the positions
    int currentX = 0;
    int currentY = 0;
    int currentBlockHFactor = 0;
    int currentBlockVFactor = 0;
    
#if DEBUGLEVEL > 20
	cout << "Now airing from the encoder ;) " << endl;
	cout << "Component: " << currentComponent << ", comp_with: " << comp_width <<
		", comp_height: " << comp_height << endl;
#endif

	
	// loop on the entire picture
	for (uint i = 0; i < comp_height; i += 8) {
		for (uint j = 0; j < comp_width; j += 8) {
			// fetch the block
#if DEBUGLEVEL > 20
			cout << "Top Left: " << currentX << ", " << currentY << ", Bottom Right: " << currentX + 8 << ", " << currentY + 8 << endl;
#endif
			uint y;
			for (y = 0; y < 8; ++y) {

				if (currentX >= width) break;
				if (currentY + y >= height) break;

				int picture_y = currentY + y;

				uint x;
				for (x = 0; x < 8; ++x) {

					if (currentX + x >= width) break;
					int realx = currentX + x;
										
					if (currentComponent == COMPONENT_Y)
						block8x8[y][x] = jpegDecoder->tCoeff_Y[picture_y][realx];
					else if (currentComponent == COMPONENT_Cb)
						block8x8[y][x] = jpegDecoder->tCoeff_Cb[picture_y][realx];
					else
						block8x8[y][x] = jpegDecoder->tCoeff_Cr[picture_y][realx];
				}
			}

			// DCPM for the DC element of the compononet (including quantization)
			for (int u = 0; u < 8; ++u) {
				for (int v = 0; v < 8; ++v) {
					block8x8[u][v] = block8x8[u][v] / quantizationTable[u][v];
					if (!u && !v) {

						// Store the DC coefficient
						prev_previousDC = block8x8[u][v];

#if DEBUGLEVEL > 20
						cout << "Current DC coefficient: " << block8x8[u][v] << ", Prev: " << previousDCCoefficient << endl;
#endif

						block8x8[u][v] = block8x8[u][v] - previousDCCoefficient;

						// quantization
						previousDCCoefficient = prev_previousDC; // set the previous DC
					}
				}
			}

			// Perform ZigZag coding, and array is ready for Huffman coding
			ZigZagCoding(block8x8, zigZagArray);

			// TODO: remove FANKOOSH
#if DEBUGLEVEL > 50
			static int fankoosh = 0;
			fankoosh++;
			if (currentComponent == COMPONENT_Cb) {
				ofstream myfile;
				std::string path_to_files = "C:/Users/y77jiang/OneDrive - University of Waterloo/5e. TCM-Inception C++/JPEG/JPEG/";
				std::string output_csv_name = path_to_files + "Nature_Cb_enc.csv";
				myfile.open(output_csv_name, std::ofstream::out | std::ofstream::app);

				std::stringstream oss;
				std::size_t found = output_csv_name.find_last_of(".");
				std::string path_with_name = output_csv_name.substr(0, found);
				found = output_csv_name.find_last_of("/\\");
				std::string name_file_only = path_with_name.substr(found + 1);

				myfile << currentX << "-" << currentY << "\n";
				int k = 0;
				for (int i = 0; i < 8; ++i) {
					for (int j = 0; j < 8; ++j) {
						//                        myfile << block8x8[i][j] << ",";
						int val = zigZagArray.at(zigZagArray.size() - 64 + k++);
						myfile << val << ",";
						//                        myfile << block8x8[i][j] << ",";
					}

					//                    if( (i + 1) < 8){
					myfile << "\n";
					//                    }
				}
				myfile.close();
			}
#endif

			// Adjust the indices:
			HScale = 1; // note: you will store the dct coefficients without any scaling
			VScale = 1; // note: algorithm below works, but you have to set the scales into 1x1
			currentX += 8 * HScale;
			currentBlockHFactor++;
			// you made a line of blocks
			if (currentBlockHFactor >= HFactor) {

				// restore the current X to its initial position and reset the counters
				currentX -= 8 * HScale * HFactor;
				currentBlockHFactor = 0;

				// go to next line
				currentY += 8 * VScale;
				currentBlockVFactor++;

				// you made a column of blocks
				if (currentBlockVFactor >= VFactor) {

					// restore the current Y to its initial position and reset the counters
					currentY -= 8 * VScale * VFactor;
					currentBlockVFactor = 0;
					currentX += 8 * HScale * HFactor;
					if (currentX >= width) {
						currentX = 0;
						currentY += 8 * VScale * VFactor;
					} // end if (currentBlockVFactor_dct[comp] >= VFactor)

				} // end  if (currentBlockVFactor_dct[comp] >= VFactor)
			} // end if(currentBlockHFactor_dct[comp] >= HFactor)
		}
	}

} // end perform_fdct



  // DCT2 - Discrete Cosine Transform - vector output
void jpeg_encoder::DCT2(int ** input, vector<vector<double>> &output)
{
	cout << "Test in DCT..." << endl << endl;
	double ALPHA, BETA;
	int row = 8;
	int col = 8;
	int u = 0;
	int v = 0;
	int i = 0;
	int j = 0;

	for (u = 0; u < row; u++)
	{
		for (v = 0; v < col; v++)
		{
			if (u == 0)
			{
				ALPHA = sqrt(1.0 / row);
			}
			else {
				ALPHA = sqrt(2.0 / row);
			}

			if (v == 0)
			{
				BETA = sqrt(1.0 / col);
			}
			else {
				BETA = sqrt(2.0 / col);
			}

			double tmp = 0.0;
			for (i = 0; i < row; i++) {
				for (j = 0; j < col; j++) {
					tmp += *((int*)input + col * i + j) * cos((2 * i + 1)*u*pi / (2.0 * row)) * cos((2 * j + 1)*v*pi / (2.0 * col));
				}
			}
			output[u][v] = ALPHA * BETA * tmp;
		}
	}

	/*cout << "the result of dct:" << endl;
	for (int m = 0; m < row; m++) {
		for (int n = 0; n < col; n++) {
			cout << setw(8) << output[m][n] << " \t";
		}
		cout << endl;
	}*/
}

float jpeg_encoder::C_dct(int u) {
    
    if(u == 0)
    {
        return 1.0f/sqrt(8.0f);
    }
    else
    {
        return sqrt(2.0 / 8.0);
    }
}


double jpeg_encoder::func_dct(int x, int y, const int block[8][8]) {
    const float PI = 3.14f;
    float sum = 0;
    for( int u = 0; u < 8; ++u)
    {
        for(int v = 0; v < 8 ; ++v)
        {
            sum += (block[u][v] * cosf( ((2*v+1) * x * PI) / 16)  * cosf( ((2*u+1) * y * PI) / 16));
        } // end inner loop
    } // end outer loop
    
    sum = C_dct(x) * C_dct(y) * sum;
    return sum;
    
}

void jpeg_encoder::perform_dct(vector<vector<double> > &outBlock, int inBlock[8][8]) {
    
    for(int y = 0; y < 8; ++y)
    {
        for(int x = 0; x < 8; ++x)
        {
            outBlock[y][x]  =  func_dct( x, y, inBlock);
            
        } // end inner loop
    } // end outer loop
    
}


char jpeg_encoder::getCategoryOfDCTCoefficient(int x) {
    
    if(x==0)
        return 0;
    else if(x==-1 || x==1)
        return 1;
    else if((x>=-3 && x<=-2)||(x>=2 && x<=3))
        return 2;
    else if((x>=-7 && x<=-4)||(x>=4 && x<=7))
        return 3;
    else if((x>=-15 && x<=-8)||(x>=8 && x<=15))
        return 4;
    else if((x>=-31 && x<=-16)||(x>=16 && x<=31))
        return 5;
    else if((x>=-63 && x<=-32)||(x>=32 && x<=63))
        return 6;
    else if((x>=-127 && x<=-64)||(x>=64 && x<=127))
        return 7;
    else if((x>=-255 && x<=-128)||(x>=128 && x<=255))
        return 8;
    else if((x>=-511 && x<=-256)||(x>=256 && x<=511))
        return 9;
    else if((x>=-1023 && x<=-512)||(x>=512 && x<=1023))
        return 10;
    else if((x>=-2047 && x<=-1024)||(x>=1024 && x<=2047))
        return 11;
    else if((x>=-4095 && x<=-2048)||(x>=2048 && x<=4095))
        return 12;
    else if((x>=-8191 && x<=-4096)||(x>=4096 && x<=8191))
        return 13;
    else if((x>=-16383 && x<=-8192)||(x>=8192 && x<=16383))
        return 14;
    else if((x>=-32767 && x<=-16384)||(x>=16384 && x<=32767))
        return 15;
    else
        return 0;
    
} // end getCategoryOfDCTCoefficient


void jpeg_encoder::ZigZagCoding(int block8x8[8][8],vector<char>&zigZagArray){
    //k- is zigZagArray index, i,j are index of matrix
    zigZagArray.push_back(block8x8[0][0]);//Take the first element
    int i=0,j=1;//Define index for matrix
    while(1){
        while(j!=0 && i!=7){//Going upside down until j!=0
            zigZagArray.push_back(block8x8[i][j]);
            i=i+1;
            j=j-1;
        }
        zigZagArray.push_back(block8x8[i][j]);//Take the edge element
        
        if(i<7)//If not last row, increment i
            i=i+1;
        
        else if(i==7)//If we hit the last row, we go right one place
            j=j+1;
        
        
        while(i!=0 && j!=7){//Going bottom up
            zigZagArray.push_back(block8x8[i][j]);
            i=i-1;
            j=j+1;
        }
        zigZagArray.push_back(block8x8[i][j]);//Take edge element
        if(j<7)//If we didn't hit the edge, increment j
            j=j+1;
        
        else if(j==7)//If we hit the last element, go down one place
            i=i+1;
        
        if(i>=7 && j>=7)//If we hit last element matrix[8][8] exit
            break;
    }
} // end ZigZagCoding


void jpeg_encoder::ZigZagCoding(vector<vector<int> > &block8x8,vector<char>&zigZagArray){
    //k- is zigZagArray index, i,j are index of matrix
    zigZagArray.push_back(block8x8[0][0]);//Take the first element
    int i=0,j=1;//Define index for matrix
    while(1){
        while(j!=0 && i!=7){//Going upside down until j!=0
            zigZagArray.push_back(block8x8[i][j]);
            i=i+1;
            j=j-1;
        }
        zigZagArray.push_back(block8x8[i][j]);//Take the edge element
        
        if(i<7)//If not last row, increment i
            i=i+1;
        
        else if(i==7)//If we hit the last row, we go right one place
            j=j+1;
        
        
        while(i!=0 && j!=7){//Going bottom up
            zigZagArray.push_back(block8x8[i][j]);
            i=i-1;
            j=j+1;
        }
        zigZagArray.push_back(block8x8[i][j]);//Take edge element
        if(j<7)//If we didn't hit the edge, increment j
            j=j+1;
        
        else if(j==7)//If we hit the last element, go down one place
            i=i+1;
        
        if(i>=7 && j>=7)//If we hit last element matrix[8][8] exit
            break;
    }
} // end ZigZagCoding


void jpeg_encoder::ZigZagCoding(double block8x8[8][8],vector<char>&zigZagArray){
    //k- is zigZagArray index, i,j are index of matrix
    zigZagArray.push_back(block8x8[0][0]);//Take the first element
    int i=0,j=1;//Define index for matrix
    while(1){
        while(j!=0 && i!=7){//Going upside down until j!=0
            zigZagArray.push_back(block8x8[i][j]);
            i=i+1;
            j=j-1;
        }
        zigZagArray.push_back(block8x8[i][j]);//Take the edge element
        
        if(i<7)//If not last row, increment i
            i=i+1;
        
        else if(i==7)//If we hit the last row, we go right one place
            j=j+1;
        
        
        while(i!=0 && j!=7){//Going bottom up
            zigZagArray.push_back(block8x8[i][j]);
            i=i-1;
            j=j+1;
        }
        zigZagArray.push_back(block8x8[i][j]);//Take edge element
        if(j<7)//If we didn't hit the edge, increment j
            j=j+1;
        
        else if(j==7)//If we hit the last element, go down one place
            i=i+1;
        
        if(i>=7 && j>=7)//If we hit last element matrix[8][8] exit
            break;
    }
} // end ZigZagCoding


void jpeg_encoder::ZigZagCoding(vector<vector<double> > &block8x8,vector<int>&zigZagArray){
    
    //k- is zigZagArray index, i,j are index of matrix
	zigZagArray.push_back(static_cast<int>(block8x8[0][0]));//Take the first element
    int i=0,j=1;//Define index for matrix
    while(1){
        while(j!=0 && i!=7){//Going upside down until j!=0
            zigZagArray.push_back(static_cast<int>(block8x8[i][j]));
            i=i+1;
            j=j-1;
        }
        zigZagArray.push_back(static_cast<int>(block8x8[i][j]));//Take the edge element
        
        if(i<7)//If not last row, increment i
            i=i+1;
        
        else if(i==7)//If we hit the last row, we go right one place
            j=j+1;
        
        
        while(i!=0 && j!=7){//Going bottom up
            zigZagArray.push_back(static_cast<int>(block8x8[i][j]));
            i=i-1;
            j=j+1;
        }
        zigZagArray.push_back(static_cast<int>(block8x8[i][j]));//Take edge element
        if(j<7)//If we didn't hit the edge, increment j
            j=j+1;
        
        else if(j==7)//If we hit the last element, go down one place
            i=i+1;
        
        if(i>=7 && j>=7)//If we hit last element matrix[8][8] exit
            break;
    }
} // end Zigzag coding


void jpeg_encoder::generateBytesAfterBITS(vector<int>&codeSize,vector<int>&huffmanValue){
    int j=0,i=1;
    while(i<=32){
        j=0;
        while(j<=255){
            if(codeSize[j]==i){
                huffmanValue.push_back(j);
            }
            j++;
        }
        i++;
    }
}

void jpeg_encoder::countNumbersOfCodesCodedWithKBits(vector<char>&BITS, vector<int>&codeSize){
    for(int i=0;i<257;i++){
        if(codeSize[i]!=0)
            BITS[codeSize[i]]++;
        
    }
}

void jpeg_encoder::adjustBitLengthTo16Bits(vector<char>&BITS){
    int i = 32, j = 0;
    
    while(1){
        if(BITS[i] > 0){
            j = i-1;
            j--;//Paziti ovdje
            while(BITS[j]<=0)
                j--;
            BITS[i]    = BITS[i]-2;
            BITS[i-1]  = BITS[i-1]+1;
            BITS[j+1]  = BITS[j+1]+2;
            BITS[j]    = BITS[j]-1;
            continue;
        }
        else{
            i--;
            if(i!=16)
                continue;
            
            while(BITS[i]==0)
                i--;
            BITS[i]--;
            return;
        }
    }
}

int jpeg_encoder::findV(vector<int>&freq, unsigned int v){
    
    // Hossam: was not INT_MAX
    int value = INT_MAX, index = -1;
    for(unsigned int i = 0;i < freq.size(); ++i){
        if(freq[i] !=0 && freq[i] <= value && i != v){
            index = i;
            value = freq[i];
        }
    }
    return index;
}


// FREQ(V) Frequency of occurrence of symbol V
// CODESIZE(V) Code size of symbol V
// OTHERS(V):  Index to next symbol in chain of all symbols in current branch of code tree
void jpeg_encoder:: generateCodeSizes (vector<int>&freq, vector<int>&codeSize, vector<int>&others) {
    int v1 = 0, v2 = 0;
    while(1){
        
//        The search for the entry with the least value of FREQ(V) selects the largest value of V with the least value of FREQ(V)
//            greater than zero.
//        The procedure “Find V1 for least value of FREQ(V1) > 0” always selects the value with the largest value of V1 when
//                more than one V1 with the same frequency occurs. The reserved code point is then guaranteed to be in the longest code
//                word category.
        // Find the first empty position
        v1 = findV(freq, -1);
        
        // Find the first empty position in your frequency vector
        v2 = findV(freq, v1);
        
        // v2 should not be negative
        if(v2==-1) {
            return;
        }
        
        freq[v1] = freq[v1] + freq[v2];
        freq[v2] = 0;
        
        codeSize[v1]++;
        
        while(others[v1] != -1){
            v1 = others[v1];
            codeSize[v1]++;
        }
        
        others[v1]=v2;
        codeSize[v2]++;
        
        while(others[v2]!=-1){
            v2 = others[v2];
            codeSize[v2]++;
        }
    }
} // end generateCodeSizes

void jpeg_encoder::generateHuffmanCodes(vector<char>&BITS,vector<int>bytes,vector<int>&byteHuffmanCode,vector<int>&byteHuffmanCodeLength){
    int code = 0;
    int k = 0;
    
    // 17 because you have code lengths until 16
    for(int i = 1 ; i< 17; ++i){
        
        for(int j = 0; j < BITS[i]; ++j){
            
            byteHuffmanCode[bytes[k]]       = code;
            byteHuffmanCodeLength[bytes[k]] = i;
            code++;
            k++;
        }
        code*=2;
    }
}

/**
 * component: DCT coefficients in zigzag manner
 * DCElementsFrequency: the frequency vector for the DC coefficient
 * ACElementsFrequency: the frequency vector for the AC coefficients
 * categoryByte: the category of each element (0-15)
 * valueByte: code or bit representation of each DCT coefficient
 */
void jpeg_encoder::generateCategoryFrequences(vector<int> &component, vector<int> &DCElementsFrequency, vector<int> &ACElementsFrequency, vector<char> &categoryByte, vector<short int> &valueByte){
   
    bool EOFhit = true;
    char category; // category of the original value
    int value; // code or bit representation of each DCT coefficient
    int counter = 0; // counter of zeros in the AC coefficients
    
    // Loop on all the coefficients in the zigzag array
    for(unsigned int i = 0; i < component.size() ; i = i + 64){
        
        EOFhit = true;
        category = getCategoryOfDCTCoefficient(component[i]);// Find category
        
        // Calculate value; if negative => orignal value is
        // http://home.elka.pw.edu.pl/~mmanowie/psap/neue/1%20JPEG%20Overview.htm <values or codes of these values>
        if(component[i] < 0) {
            value = pow(2., category) + component[i] - 1;
        }
        else {
            value = component[i];
        }
        
        // Increment frequency of DC element
        DCElementsFrequency[category]++;
        
        // Push back category byte and calculated value. These arrays will help me while writing down the file data
        categoryByte.push_back(category);
        
        // push back value
        valueByte.push_back(value);
        
        // AC coefficients:
        for(int j = 1; j < 64; j++) {
            
            // if AC component is not zero, write down: 0x0+category value
            if(component[i+j] != 0) {
                
                // Get category of component element
                category = getCategoryOfDCTCoefficient(component[i+j]);
                
                // Calculate code or bit representation of each AC coefficient
                if(component[i+j] < 0) {
                    value = pow(2.,category) + component[i+j] - 1;
                }
                else {
                    value = component[i+j];
                }
                
                // Pushback category and value
                categoryByte.push_back(category);
                valueByte.push_back(value);
                ACElementsFrequency[category]++;//Increment frequency of category byte
                continue;
            }
            
            // Run Length Coding: If all elements until end are zeros, then write EOF element 0x00
            for(int k = j; k < 64 ; ++k) {
                if (component[i+k] != 0){
                    EOFhit = false;
                    break;
                }
            }
            
            if (EOFhit) {
                
                // Increment frequency of first (0x00) element
                ACElementsFrequency[0]++;
                
                // Push back category and value (code representation)
                categoryByte.push_back(0);
                valueByte.push_back(0);
                break; // All elements are zeros, go to another 8x8 block
                
            }
            else{
                
                // All elements are not zero so we must count how many zeros are before non zero AC element
                while(component[i+j]==0) {
                    
                    // If we count 16 zeros, we must insert new sign 0xF0 which means 16 zeros
                    if(counter==16) {
                        
                        ACElementsFrequency[0xF0]++;//Increment frequency for element 0xF0
                        categoryByte.push_back((char)0xF0);// Push back category and value
                        valueByte.push_back(0);
                        counter = -1;// Restart counter to -1 because it will increment below
                    }
                    
                    counter++;
                    j++; // move to the next coefficient
                }
                
                // We counted x zeros. Format is (number of zeros,category value)= 1 byte
                category = getCategoryOfDCTCoefficient(component[i+j]);// we get category
                categoryByte.push_back( (counter << 4 ) + category);// Shift category in upper 4 bits
                
                // We test if the component value is < than zero
                if(component[i+j] < 0) {
                    valueByte.push_back(pow(2.,category) + component[i+j] - 1);
                }
                else {
                    valueByte.push_back(component[i+j]);
                }
                
                ACElementsFrequency[ (counter<<4) + category]++;//Increment frequency
                
                counter = 0;// Reset counter to go to the next 64 coefficients
                EOFhit = true;
            }
        }
        
        // End of Block signature
        categoryByte.push_back(EOB_SIGNATURE);
        valueByte.push_back(EOB_SIGNATURE);
        // TODO: remove this
//        categoryByte.push_back(-5);
//        valueByte.push_back(-5);
        
    }
}



void jpeg_encoder::writeQuantizationTablesInFile(ofstream &file,vector<char> &table,int tableID){
    /*Quantization table-specification syntax:
     *                DQT    Lq   Pq  Tq  [Q0  Q1  Q2  ...  Q63]
     *Number of bits: 16     16    4  4    8   8   8          8
     *
     *
     *   DQT - Define Quantization Table marker - Marks the beginning of quantization table-specification parameters (FFDB)
     *   Lq - Quantization table definition length - Specifies the length of all quantization table parameters
     *
     *   Pq - Quantization table element precision - Specifies the precision of the Qk values. Value 0 indicates 8-bit Qk
     *   values; value 1 indicates 16-bit Qk values. Pq shall be zero for 8 bit sample precision
     *   Tq: Quantization table destination identifier - Specifies one of four possible destinations at the decoder into
     *   which the quantization table shall be installed.
     *   Qk: Quantization table element - Specifies the kth element out of 64 elements, where k is the index in the zigzag
     *    ordering of the DCT coefficients. The quantization elements shall be specified in zig-zag scan order.
     */
    char a=(char)0xFF;
    file.write((char*)&a, 1);
    a=(char)0xDB;
    file.write((char*)&a, 1);
    a=0x00;
    file.write((char*)&a, 1);
    a=0x43;
    file.write((char*)&a, 1);
    if(tableID==0){
        a=0x00;
        file.write((char*)&a, 1);
    }
    else{
        a=0x01;
        file.write((char*)&a, 1);
    }
    for(unsigned int i=0;i<table.size();i++){
        file.write((char*)&table[i], 1);
    }
}


void jpeg_encoder::emit_DQT(ofstream &file) {
	/*Quantization table-specification syntax:
	*                DQT    Lq   Pq  Tq  [Q0  Q1  Q2  ...  Q63]
	*Number of bits: 16     16    4  4    8   8   8          8
	*
	*
	*   DQT - Define Quantization Table marker - Marks the beginning of quantization table-specification parameters (FFDB)
	*   Lq - Quantization table definition length - Specifies the length of all quantization table parameters
	*
	*   Pq - Quantization table element precision - Specifies the precision of the Qk values. Value 0 indicates 8-bit Qk
	*   values; value 1 indicates 16-bit Qk values. Pq shall be zero for 8 bit sample precision
	*   Tq: Quantization table destination identifier - Specifies one of four possible destinations at the decoder into
	*   which the quantization table shall be installed.
	*   Qk: Quantization table element - Specifies the kth element out of 64 elements, where k is the index in the zigzag
	*    ordering of the DCT coefficients. The quantization elements shall be specified in zig-zag scan order.
	*/

	emit_marker(M_DQT, file);
	int comp_size = static_cast<int>(jpegDecoder->components.size());

	// Current counter
	int counter = COMPONENT_Y;
	
	// Two quantization tables at max for {Y}, {Cb, Cr}
	do
	{ 
		QuantizationTable * qTable = jpegDecoder->components[counter].componentQuantizationTable;
		int tableLength = qTable->tableLength;

		if (counter == 0) {
			emit_byte(0, file);
			emit_byte(tableLength, file);
		}
		else if (tableLength <= DQT_LENGTH_WITH_TWO_MARKERS) {
			emit_marker(M_DQT, file);
			emit_byte(0, file);
			emit_byte(tableLength, file);
		}

		// Write the tableID
		uint_8 tableID = counter;
		emit_byte(tableID, file);

		// Write the QTable values
		vector <char> qtable_vec;

		if (counter == COMPONENT_Y) {
			ZigZagCoding(quantization_table_write_process_luminance.quantizationTableData, qtable_vec);
		}
		else {
			ZigZagCoding(quantization_table_write_process_chrominance.quantizationTableData, qtable_vec);
		}

		for (int i = 0; i < qtable_vec.size(); ++i) {
			emit_byte(qtable_vec.at(i), file);
		}
		qtable_vec.clear();

		// Increment the counter
		counter++;
	} while (counter < comp_size - 1);

} // end write_dqt

//void jpeg_encoder::writeStartOfFileByteInFile(ofstream &file){
//    char a=(char)0xFF;
//    file.write((char*)&a, 1);
//    a=(char)0xD8;
//    file.write((char*)&a, 1);
//}


void jpeg_encoder::writeStartOfFileByteInFile(ofstream &file){
    emit_marker(M_SOI, file);
}


// Write huffman tables from the decoder:
void jpeg_encoder::writeHuffmanTablesFromDecoder(ofstream &file, vector<char>&BITS, vector<int>&valuesDC, vector<char>&BITSA, vector<int>&valuesAC, bool isLuminance) {
    
    
    // First, calculate the table length (Lh)
    uint_16 table_length = 0;
    
    // Right most 4 bits of the byte
    uint_8  tableID = 0; // Specifies one of component: 0 for luminance and 1 for chrominance
    
    
    // Left most 4 bits of the byte
    uint_8  tableClass = 0; // Specifies is it DC element or AC element of table. 0-DC element 1-AC element
    uint_8  huffmanTableOptions = 0; // I will decompose this in two nibbles
    
    // Read components and their huffman tables
    for (int i = 0; i < jpegDecoder->huffmanTables.size(); ++i) {
        
        const HuffmanTable* hTable = jpegDecoder->huffmanTables.at(i);
        
        // Emit the DHT marker
        emit_marker(M_DHT, file);
        
        // table length:
        table_length = hTable->tableSegmentLengthFromBitstream;
        emit_word(table_length, file);
        
        
        tableID      = hTable->tableID;
        tableClass   = hTable -> tableClass;
        huffmanTableOptions = (tableClass << 4 ) | (tableID);
        emit_byte(huffmanTableOptions, file);
        
        // Next 16 bytes are number of elements coded with 1-16 bits
        for(int j = 0; j < 16; ++j)
        {
            emit_byte(hTable->number_of_codes_for_each_1to16[j], file);
        }
        
        // Remaining bytes are the data values to be mapped
        // Build the Huffman map of (length, code) -> value
        // Once the map has been built, emit it out
        std::map<huffKey, uint_8>::const_iterator iter_new;
        for (iter_new = hTable->huffData.begin();iter_new != hTable->huffData.end(); ++iter_new) {
            
            // Print Code - Its Length : Equivalent letter
            uint_8 element = iter_new->second;
            emit_byte(element, file);
        }
        
    }
}


void jpeg_encoder::writeHuffmanTables(ofstream &file, vector<char>&BITS, vector<int>&valuesDC, vector<char>&BITSA, vector<int>&valuesAC, bool isLuminance){
    /*General definition of Huffman tables looks like:
     *      DHT  Lh  Tc Th [L1  L2  L3  ...  L16][V11  V12  ...  V1L1 V21 V22 ... V2L2  V161 V162 ... V16L16]
     *              |                                                                                       |
     *              |                                                                                       |
     *              +--------------------------------Multiply with number of huffman tables-----------------+
     *
     *              Number of Huffman tables is 4: Luminance DC and AC + Chrominance DC and AC
     *
     *   DHT=FFC4 Start of Define Huffman Table marker
     *   Lh - Huffman table length in bytes (starting from Lh byte) 16 bits long
     *   Tc - Table class. If we are writing DC elements then this nibble should be 0, in other case 1 ( 4 bits long)
     *   Th - Huffman table destination identifier. 0 for luminance table, 1 for chrominance table
     *   Lk - Nuber of bits coded with k bits (8 bits)
     *   Vi,j - Value associated with each Huffman code - Specifies, for each i, the value associated with each Huffman
     *    code of length i. The meaning of each value is determined by the Huffman coding model.(8 bits)
     */
    
    //First, lets count Lh value
    bool isFound = false;
    int m = 0;
    for(unsigned int i=0;i < BITS.size(); ++i) {
        m+=BITS[i];
    }
    
    for(unsigned int i=0;i<BITSA.size(); ++i) {
        m+=BITSA[i];
    }
    
    // Now I have to add 17 bytes for luminance DC and 17 bytes for luminance AC. 17 comes from (Tc + Th=1 byte + (L1+L2+...+L16=16 bytes))*2 because of DC and AC elements=17 bytes
    m+=17*2;
    // Finally we have to add two bytes because Lh itself has 2 bytes
    m+=2;
    char byte = (char)0xFF;
    file.write((char*)&byte, 1);
    byte = (char)0xC4;
    file.write((char*)&byte, 1);
    
    byte = (m&0x0000FFFF)>>8;
    file.write((char*)&byte, 1);
    byte = (m&0x000000FF);
    file.write((char*)&byte, 1);
    
    if(isLuminance){
        byte=(char)0x00;//Luminance DC
        file.write((char*)&byte, 1);
    }
    else{
        byte=(char)0x01;
        file.write((char*)&byte, 1);
    }
    
    //Writing LUMINANCE DC ELEMENTS
    for(int i = 1 ; i < 17; ++i){
        
        if(BITS[i]!=0){
            byte=BITS[i];
            file.write((char*)&byte, 1);
            isFound=true;
        }
        
        if(isFound==false){
            byte=0x00;
            file.write((char*)&byte, 1);
        }
        isFound=false;
    }
    
    for(unsigned int i = 0; i < valuesDC.size(); ++i) {
        byte=valuesDC[i]&0x000000FF;
        file.write((char*)&byte, 1);
    }
    
    if(isLuminance){
        byte=0x10;//Luminance AC
        file.write((char*)&byte, 1);
    }
    else{
        byte=0x11;
        file.write((char*)&byte, 1);
    }
    
    for(int i = 1 ; i < 17; ++i){
        if(BITSA[i]!=0){
            byte=BITSA[i];
            file.write((char*)&byte, 1);
            isFound=true;
        }
        
        if(isFound==false){
            byte=0x00;
            file.write((char*)&byte, 1);
        }
        isFound=false;
    }
    
    for(unsigned int i=0;i < valuesAC.size(); ++i){
        byte = valuesAC[i]&0x000000FF;
        file.write((char*)&byte, 1);
    }
}



void jpeg_encoder::writeScanHeaderMarker(ofstream &file){
    /*Scan header looks like:
     *
     *
     *                    SOS   Lh  Ns [Cs1 Td1 Ta1][Cs2 Td2 Ta2]...[Csn Tdn Tan]  Ss Se Ah Al
     *
     *Number of bits:     16    16  8    8   4    4   8   4   4       8   4   4    8   8  4  4
     *
     *SOS: Start of scan marker - Marks the beginning of the scan parameters.
     *Lh :Scan header length - Specifies the length of the scan header
     *Ns: Number of image components in scan - Specifies the number of source image components in the scan. The
     *value of Ns shall be equal to the number of sets of scan component specification parameters (Csj, Tdj, and Taj)
     *present in the scan header.
     *
     *Csj - Scan component selector - Selects which of the Nf image components specified in the frame parameters
     *shall be the jth component in the scan. Each Csj shall match one of the Ci values specified in the frame header,
     *and the ordering in the scan header shall follow the ordering in the frame header. If Ns > 1, the order of
     *interleaved components in the MCU is Cs1 first, Cs2 second, etc.
     *
     *Tdj: DC entropy coding table destination selector - Specifies one of four possible DC entropy coding table
     **destinations from which the entropy table needed for decoding of the DC coefficients of component Csj is
     *retrieved
     *
     **Taj: AC entropy coding table destination selector - Specifies one of four possible AC entropy coding table
     *destinations from which the entropy table needed for decoding of the AC coefficients of component Csj is
     *retrieved
     *
     *Ss: Start of spectral or predictor selection - In the DCT modes of operation, this parameter specifies the first
     *DCT coefficient in each block in zig-zag order which shall be coded in the scan. This parameter shall be set to
     *zero for the sequential DCT processes
     *
     *Se: End of spectral selection - Specifies the last DCT coefficient in each block in zig-zag order which shall be
     **coded in the scan. This parameter shall be set to 63 for the sequential DCT processes. In the lossless mode of
     *operations this parameter has no meaning. It shall be set to zero.
     *
     *Ah: Successive approximation bit position high - This parameter specifies the point transform used in the
     *preceding scan (i.e. successive approximation bit position low in the preceding scan) for the band of coefficients
     **specified by Ss and Se. This parameter shall be set to zero for the first scan of each band of coefficients. In the
     *lossless mode of operations this parameter has no meaning. It shall be set to zero.
     *
     *Al: Successive approximation bit position low or point transform - In the DCT modes of operation this
     ***parameter specifies the point transform, i.e. bit position low, used before coding the band of coefficients
     *specified by Ss and Se. This parameter shall be set to zero for the sequential DCT processes. In the lossless
     *mode of operations, this parameter specifies the point transform, Pt.
     */
    //Write FFDA
    char byte=(char)0xFF;
    file.write((char*)&byte, 1);
    byte=(char)0xDA;
    file.write((char*)&byte, 1);
    
    //Write Lh. There is 12 bytes in Scan header
    char *bytes=new char[12];
    bytes[0]=(char)0x00;
    bytes[1]=(char)0x0C;
    
    //Three components: YCbCr
    bytes[2]=(char)0x03;
    bytes[3]=(char)0x01;//Component Y ID
    bytes[4]=(char)0x00;
    bytes[5]=(char)0x02;//Component Cb ID
    bytes[6]=(char)0x11;
    bytes[7]=(char)0x03;//Component Cr ID
    bytes[8]=(char)0x11;
    bytes[9]=(char)0x00;//Ss=0
    bytes[10]=(char)0x3F;//Se=3F=63
    bytes[11]=(char)0x00;//Ah=Al=0
    file.write((char*)bytes, sizeof(char)*12);
    
    delete[] bytes;    
}

void jpeg_encoder::writeEOFMarker(ofstream &file){
    emit_marker(M_EOI, file); 
}

void jpeg_encoder::writeBaselineDCTInformations(ofstream &file){
    
    /*SOFn: Start of frame marker - Marks the beginning of the frame parameters. The subscript n identifies whether
     *the encoding process is baseline sequential, extended sequential, progressive, or lossless, as well as which
     *entropy encoding procedure is used.*/
    /*For Baseline DCT process, SOFn frame marker is FFC0*/
    /*Here is general scheme of SOFn block of data*/
    /*FFC0  Lf  P  Y   X   Nf  [C1 H1 V1 Tq1] [C2 H2 V2 Tq2] ......[Cn Hn Vn Tqn]
     *Number of bits    16   16  8  16  16  8    8  4  4   8
     *FFC0 is start of Baseline DCT marker
     *Lf - frame header length
     *P - precision
     *Y - Height
     *X - Width
     *Nf - Number of components (For our case is 3 (YCbCr))
     *C1 - Component ID
     *H1 - Horisontal sampling factor (usind in chroma subsamping)
     *V1 - Vertical sampling factor (usind in chroma subsamping)
     *Tq1- Quantization table ID used for C1 component*/
    char byte=(char)0xFF;
    file.write((char*)&byte, 1);
    byte=(char)0xC0;
    file.write((char*)&byte, 1);
    byte=(char)0x00;
    file.write((char*)&byte, 1);
    byte=(char)0x11;//Lf=17 bytes
    file.write((char*)&byte, 1);
    byte=(char)0x08;//P=8
    file.write((char*)&byte, 1);
    
    // Y is 16 bits long. I keep height in int variable so I need to do some calculations
    byte=(image_to_export_height&0x0000FFFF)>>8;
    file.write((char*)&byte, 1);
    byte=(image_to_export_height&0x000000FF);
    file.write((char*)&byte, 1);
    
    // X is 16 bits long. I keep height in int variable so I need to do some calculations
    byte=(image_to_export_width&0x0000FFFF)>>8;
    file.write((char*)&byte, 1);
    byte=(image_to_export_width&0x000000FF);
    file.write((char*)&byte, 1);
    
    // There are three components YCbCr so Nf=3
    byte=(char)0x03;
    file.write((char*)&byte, 1);
    
    byte=(char)0x01;//Y component ID is 01
    file.write((char*)&byte, 1);
    byte=(char)0x22;//H=1 and V=1
    file.write((char*)&byte, 1);
    byte=(char)0x00;//Table ID for Y is 00
    file.write((char*)&byte, 1);
    
    
    // TODO: need to change the subsampling here (adaptively), qTables (qTableID)
    byte=(char)0x02;//Cb component ID is 02
    file.write((char*)&byte, 1);
    byte=(char)0x11;//H=1 and V=1
    file.write((char*)&byte, 1);
    byte=(char)0x01;//Quantization table ID=01
    file.write((char*)&byte, 1);
    
    byte=(char)0x03;//Cr component ID is 03
    file.write((char*)&byte, 1);
    byte=(char)0x11;//H=1 and V=1
    file.write((char*)&byte, 1);
    byte=(char)0x01;//Quantization table ID=01
    file.write((char*)&byte, 1);
}

void jpeg_encoder::emit_start_markers(ofstream &file){
    
    // Write Start of File marker
    writeStartOfFileByteInFile(file);
    
    // Write the appliation signature:
    write_jfif_app0(file);
    
    // Write the DQT:
	emit_DQT(file);

    // Write the SOF:
    emit_sof(file);
    
} // end emit_start_markers


void jpeg_encoder::emit_sof(ofstream &file) {
    
    /*SOFn: Start of frame marker - Marks the beginning of the frame parameters. The subscript n identifies whether
     *the encoding process is baseline sequential, extended sequential, progressive, or lossless, as well as which
     *entropy encoding procedure is used.*/
    /*For Baseline DCT process, SOFn frame marker is FFC0*/
    /*Here is general scheme of SOFn block of data*/
    /*FFC0  Lf  P  Y   X   Nf  [C1 H1 V1 Tq1] [C2 H2 V2 Tq2] ......[Cn Hn Vn Tqn]
     *Number of bits    16   16  8  16  16  8    8  4  4   8
     *FFC0 is start of Baseline DCT marker
     *Lf - frame header length
     *P - precision
     *Y - Height
     *X - Width
     *Nf - Number of components (For our case is 3 (YCbCr))
     *C1 - Component ID
     *H1 - Horisontal sampling factor (usind in chroma subsamping)
     *V1 - Vertical sampling factor (usind in chroma subsamping)
     *Tq1- Quantization table ID used for C1 component*/
    
    // TODO: getters and setters
    uint_8 m_num_components  = static_cast<uint_8>(jpegDecoder->components.size());
    uint_8 precision         = jpegDecoder->jpegImageSamplePrecision;
    uint_16 height           = image_to_export_height;
    uint_16 width            = image_to_export_width;
   
    emit_marker(M_SOF0, file);                           /* baseline */
    emit_word(3 * m_num_components + 2 + 5 + 1, file); // length of the header
    emit_byte(precision, file);                                  /* precision */
    emit_word(height, file);
    emit_word(width, file);
    emit_byte(m_num_components, file);
    for (int i = 0; i < m_num_components; i++) {
        emit_byte(static_cast<uint_8>(i + 1), file);                                   /* component ID     */
        
        uint_8 HFactor = static_cast<uint_8>(jpegDecoder->components.at(i).HFactor);
        uint_8 VFactor = static_cast<uint_8>(jpegDecoder->components.at(i).VFactor);
        uint_8 qTableID = static_cast<uint_8>(jpegDecoder->components.at(i).componentQuantizationTable->tableID);
        emit_byte((HFactor << 4) + VFactor, file);  /* h and v sampling */
        
        emit_byte(qTableID, file);     /* quant. table num */
        
        
    }

}


void jpeg_encoder::emit_sos(ofstream &file) {
    
    /*Scan header looks like:
     *
     *
     *                    SOS   Lh  Ns [Cs1 Td1 Ta1][Cs2 Td2 Ta2]...[Csn Tdn Tan]  Ss Se Ah Al
     *
     *Number of bits:     16    16  8    8   4    4   8   4   4       8   4   4    8   8  4  4
     *
     *SOS: Start of scan marker - Marks the beginning of the scan parameters.
     *Lh :Scan header length - Specifies the length of the scan header
     *Ns: Number of image components in scan - Specifies the number of source image components in the scan. The
     *value of Ns shall be equal to the number of sets of scan component specification parameters (Csj, Tdj, and Taj)
     *present in the scan header.
     *
     *Csj - Scan component selector - Selects which of the Nf image components specified in the frame parameters
     *shall be the jth component in the scan. Each Csj shall match one of the Ci values specified in the frame header,
     *and the ordering in the scan header shall follow the ordering in the frame header. If Ns > 1, the order of
     *interleaved components in the MCU is Cs1 first, Cs2 second, etc.
     *
     *Tdj: DC entropy coding table destination selector - Specifies one of four possible DC entropy coding table
     **destinations from which the entropy table needed for decoding of the DC coefficients of component Csj is
     *retrieved
     *
     **Taj: AC entropy coding table destination selector - Specifies one of four possible AC entropy coding table
     *destinations from which the entropy table needed for decoding of the AC coefficients of component Csj is
     *retrieved
     *
     *Ss: Start of spectral or predictor selection - In the DCT modes of operation, this parameter specifies the first
     *DCT coefficient in each block in zig-zag order which shall be coded in the scan. This parameter shall be set to
     *zero for the sequential DCT processes
     *
     *Se: End of spectral selection - Specifies the last DCT coefficient in each block in zig-zag order which shall be
     **coded in the scan. This parameter shall be set to 63 for the sequential DCT processes. In the lossless mode of
     *operations this parameter has no meaning. It shall be set to zero.
     *
     *Ah: Successive approximation bit position high - This parameter specifies the point transform used in the
     *preceding scan (i.e. successive approximation bit position low in the preceding scan) for the band of coefficients
     **specified by Ss and Se. This parameter shall be set to zero for the first scan of each band of coefficients. In the
     *lossless mode of operations this parameter has no meaning. It shall be set to zero.
     *
     *Al: Successive approximation bit position low or point transform - In the DCT modes of operation this
     ***parameter specifies the point transform, i.e. bit position low, used before coding the band of coefficients
     *specified by Ss and Se. This parameter shall be set to zero for the sequential DCT processes. In the lossless
     *mode of operations, this parameter specifies the point transform, Pt.
     */
    
    // TODO: getters and setters
    uint_8 m_num_components  = static_cast<uint_8>(jpegDecoder->components.size());
    
    emit_marker(M_SOS, file); // marker
    emit_word(2 * m_num_components + 2 + 1 + 3, file); // header length
    emit_byte(m_num_components, file); // number of components
    for (int i = 0; i < m_num_components; i++) {
        emit_byte(static_cast<uint_8>(i + 1), file); // component ID
        
        // TODO: getters
        uint_8 tableDC = jpegDecoder->componentTablesDC[i]->tableID;
        uint_8 tableAC = jpegDecoder->componentTablesAC[i]->tableID;
        
        if (i == 0) {
             emit_byte((tableDC << 4) + tableAC, file); // component huffman table (left part is DC, right part is AC)
//            emit_byte((0 << 4) + 0, file); // component huffman table (left part is DC, right part is AC)
        } else {
             emit_byte((tableDC << 4) + tableAC, file); // component huffman table (left part is DC, right part is AC)
//            emit_byte((1 << 4) + 1, file);
        }
    }
    
    // TODO: getters
    uint_8 zigStart  = jpegDecoder->zigZagStart;
    uint_8 zigEnd    = jpegDecoder->zigZagEnd;
    uint_8 dummyByte = 0;
    
    emit_byte(zigStart, file);     /* spectral selection */
    emit_byte(zigEnd, file);
    emit_byte(dummyByte, file); // TODO: Bit approximation for progressive JPEG
}


void jpeg_encoder::write_jfif_app0(ofstream &file) {
    
    emit_marker(M_APP0, file);
    emit_word(2 + 4 + 1 + 2 + 1 + 2 + 2 + 1 + 1, file);
    emit_byte(0x4A, file); emit_byte(0x46, file); emit_byte(0x49, file); emit_byte(0x46, file); /* Identifier: ASCII "JFIF" */
    emit_byte(0, file);
    emit_byte(1, file);      /* Major version */
    emit_byte(1, file);      /* Minor version */
    emit_byte(0, file);      /* Density unit */
    emit_word(1, file);
    emit_word(1, file);
    emit_byte(0, file);      /* No thumbnail image */
    emit_byte(0, file);
    
}



void jpeg_encoder::encodeImageEntryPoint(vector<int> luminanceZigZagArray,
                                         vector<int> chrominanceCbZigZagArray, vector<int> chrominanceCrZigZagArray, ofstream &file) {
    
    // Y is the original hFactor and vFactor; others are subsampled
    int hFactor = jpegDecoder->components[COMPONENT_Y].HFactor;
    int vFactor = jpegDecoder->components[COMPONENT_Y].VFactor;
    
    
    // MCU: minimum coding unit
    int xstride_by_mcu = 8 * hFactor; // mcu width
    int ystride_by_mcu = 8 * vFactor; // mcu height
    
    // Just encode the image by 'macroblock' (size is 8x8, 8x16, or 16x16)
	int counter = 0;
    for (int y = 0 ; y < image_to_export_height; y+=ystride_by_mcu)
    {
        for(int x = 0; x < image_to_export_width; x+=xstride_by_mcu)
        {
            encode_mcu(luminanceZigZagArray, chrominanceCbZigZagArray, chrominanceCrZigZagArray, hFactor, vFactor, x, y, file);
			counter++;
			// cout << x << "---" << y << endl;
        }
	}
	cout << counter << endl;
	// append the last few bits that are left
	if (m_bits_in > 0 && m_bits_in < 8) {
		uint_8 byte_last = m_bit_buffer >> (32 - m_bits_in);
		byte_last <<= (8 - m_bits_in);
		byte_last |= ((1 << (8 - m_bits_in)) - 1);

		emit_byte(byte_last, file);
		if (byte_last == 0xFF) {
			emit_byte(0, file); // Byte Stuffing
		}
	}
}

void jpeg_encoder::encode_mcu(vector<int> luminanceZigZagArray, vector<int> chrominanceCbZigZagArray, vector<int> chrominanceCrZigZagArray, int componentWidth, int componentHeight, int start_x, int start_y, ofstream &file) {
	//cout << "height:" << componentHeight << " Width: " << componentWidth << endl;
    for (int y = 0; y < componentHeight; ++y)
    {
        for(int x = 0; x < componentWidth; ++x)
        {
            
            int luma_x = start_x + x*8;
            int luma_y = start_y + y*8;
           
            // encode block
            encode_block(luminanceZigZagArray, luma_x, luma_y, COMPONENT_Y, count_block_Y, file);
            count_block_Y++;		
        }

    }
    
    int numberOfComponents = static_cast<int>(jpegDecoder->components.size());
    // The rest of the components if they exist:
    for(int iComponent = 1; iComponent < numberOfComponents; ++iComponent)
    {
        
        int chroma_x = start_x / componentWidth;
        int chroma_y = start_y / componentHeight;

        // encode block
        if(iComponent == COMPONENT_Cb) {

            encode_block(chrominanceCbZigZagArray, chroma_x, chroma_y, COMPONENT_Cb, count_block_Cb, file);  
            count_block_Cb++;
        }
        else if(iComponent == COMPONENT_Cr) {

            encode_block(chrominanceCrZigZagArray, chroma_x, chroma_y, COMPONENT_Cr, count_block_Cr, file);        
            count_block_Cr++;
			
        }		
    }
}

void jpeg_encoder::encode_block(vector<int> zigZagArray, int CurrentX, int CurrentY, int currentComponent, int count_block, ofstream &file) {
    
    // DC coding
#if DEBUGLEVEL > 20
	if(currentComponent == COMPONENT_Y)
     cout << "Encode block at X: " << CurrentX << ", Y: " << CurrentY << endl;
#endif
    
	//cout << total_block << endl;
    int start_index = returnIndexInZigZagArray(count_block);
    const int dc_delta = zigZagArray.at(start_index);


#if DEBUGLEVEL > 20
	if (currentComponent == COMPONENT_Y && CurrentX >= 224 && CurrentY == 0)
	{	
		int init = start_index;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {

				int idx = start_index + j + i * 8;
				cout << zigZagArray.at(idx) << ", ";
			}
			cout << "\n";
		}

	}
#endif   
    
#if DEBUGLEVEL > 20
    cout << "DC Delta " << dc_delta << " at start index: " << start_index << "\n" << endl;
#endif
    
    // Encode DC delta coefficient
    // 1. Write the code for the DC delta from Huffman DC table of current component with the its corresponding codeLength
    // (table->code[bit_count(dc_delta)], table->codeLength[bit_count(dc_delta)]
    // 2. Write the signed int bits for the dc_delta itself write(dc_delta, nbits)
    
    HuffmanTable* dc_hTable = jpegDecoder->componentTablesDC[currentComponent];
    
    const uint nbits = bit_count(dc_delta);
    // put_bits:
    put_bits(dc_hTable->codes[nbits], dc_hTable->codeLengths[nbits], file);
    put_signed_int_bits(dc_delta, nbits, file);
	// remove
	// cout << dc_hTable->codes[nbits] << "----" << dc_hTable->codeLengths[nbits] << endl;
	// cout << dc_delta << "====" << nbits << endl;
#if DEBUGLEVEL >20
	long pos = file.tellp();
	if (pos >= 5555522) {
 		cout << pos << endl;
		cout << "# of bits:" << nbits << endl;
		cout << dc_hTable->codes[nbits] <<"--"<< dc_hTable->codeLengths[nbits] << endl;
		cout << "postion X:" << CurrentX << ", position Y:" << CurrentY << endl;
	}
#endif
    // ----------
    
    // Encode AC coefficients:
    HuffmanTable* ac_hTable = jpegDecoder->componentTablesAC[currentComponent];
    
    int run_len = 0;
    
    for(int i = start_index + 1; i < start_index + 64; i++) {
        const short ac_val = zigZagArray.at(i);
        
        if(ac_val == 0) {
            run_len++;
        }
        else {
            // 16 zeros case
            while (run_len >= 16)
            {
                // Write bits (Huffman code of 0xF0 and it's code Length 0XF0) for 16 zeros
                // put_bits:
                put_bits(ac_hTable->codes[0xF0], ac_hTable->codeLengths[0xF0], file);
                
                run_len -= 16;
            }
			const uint nbits = bit_count(ac_val);
			const int code = (run_len << 4) + nbits; // left part is the run, and right part is the nBits of the next AC

		// Write HAC_codes[code], HAC_codeLength[code]
		// put_bits:
			put_bits(ac_hTable->codes[code], ac_hTable->codeLengths[code], file);
			put_signed_int_bits(ac_val, nbits, file);
			run_len = 0;
#if DEBUGLEVEL >20
			long pos = file.tellp();
			if (pos >= 522) {
				cout << pos << endl;
				cout << "# of bits:" << nbits << endl;
				cout << "AC code " << std::hex << code << endl;
				cout << ac_hTable->codes[code] << "--" << ac_hTable->codeLengths[code] << endl;
				cout << std::dec << "postion X:" << CurrentX << ", position Y:" << CurrentY << endl;
			}
#endif
        }
        
    } // end for
    
    // If there is still a run
    if(run_len) {
        // Write the EOB or 000 code from AC huffman table HAC[0] code, HAC[0] codeLength
        // put_bits:
        // EOB
        put_bits(ac_hTable->codes[0], ac_hTable->codeLengths[0], file);

#if DEBUGLEVEL >20
		long pos = file.tellp();
		if (pos >= 522) {
			cout << pos << endl;
			cout << "# of bits:" << nbits << endl;
			//cout << "AC code " << std::hex << code << endl;
			cout << ac_hTable->codes[0] << "--" << ac_hTable->codeLengths[0] << endl;
			cout << std::dec << "postion X:" << CurrentX << ", position Y:" << CurrentY << endl;
		}
#endif
    }
    
}


int jpeg_encoder::returnIndexInZigZagArray(int count_block)
{
    return 64 * count_block;
}


// not used: that's if you stored blocks raster and not the way you did
int jpeg_encoder::returnIndexInZigZagArray(int x, int y)
{
    
    return 64 * (y / 8 * image_to_export_width / 8 + x / 8);
}

void jpeg_encoder::put_signed_int_bits(int bits, uint_32 bits_length, ofstream &file) {
    
    if (bits < 0) {
        bits--;
    }

    put_bits(bits & ((1 << bits_length) - 1), bits_length, file);
}

void jpeg_encoder::put_bits(uint bits, uint_32 bits_length, ofstream &file) {
    
    // Add the bits to your buffer
    uint_32 bits_cast = static_cast<uint_32>(bits);
    m_bit_buffer      = m_bit_buffer | (bits_cast << (32 - (m_bits_in += bits_length)));
    
    while(m_bits_in >= 8) {
        
        uint_8 byte = static_cast<uint_8> ( ( (m_bit_buffer >> 24) & 0xFF) );
        emit_byte(byte, file);
        
        if(byte == 0xFF) {
            emit_byte(0, file); // Byte Stuffing
        }
        
        // Left Shift the buffer one byte
        m_bit_buffer <<= 8;
        
        // Decrease the amount of m_bits_in  by 8
        m_bits_in -= 8;
    } // end while
}

bool jpeg_encoder::savePicture() {
    
    cout << "\n\n Encode Start " << endl;
    
    // Copy the quantization tables
    copy_qTables();
    
    
    //In these arrays I will keep my data after FDCT
    vector<int> luminanceZigZagArray;
    vector<int> chrominanceCbZigZagArray;
    vector<int> chrominanceCrZigZagArray;
    
    
    //Prosess every component with FDCT
    uint_8 ** luminance     = jpegDecoder->m_YPicture_buffer;
    uint_8 ** chrominanceCb = jpegDecoder->m_CbPicture_buffer;
    uint_8 ** chrominanceCr = jpegDecoder->m_CrPicture_buffer;
    
    
    perform_fdct(luminance, luminanceZigZagArray, quantization_table_write_process_luminance.quantizationTableData, COMPONENT_Y);
	int numberofComponent = jpegDecoder->components.size();
	if (numberofComponent > 1) {
		perform_fdct(chrominanceCb, chrominanceCbZigZagArray, quantization_table_write_process_chrominance.quantizationTableData, COMPONENT_Cb);
		perform_fdct(chrominanceCr, chrominanceCrZigZagArray, quantization_table_write_process_chrominance.quantizationTableData, COMPONENT_Cr);
	}
    
    
    
    // Here starts file writing process:
    string fileName = image_to_export_filename;
    ofstream output(fileName.c_str(), ios::out | ios::binary);
    
    // emit start markers
    emit_start_markers(output);

    vector<char>chrominanceDCBITS(33,0);
    vector<char>chrominanceACBITS(33,0);
    vector<int>chrominanceDCHuffmanValues;
    vector<int>chrominanceACHuffmanValues;
    
    
    // Write decoder huffman tables:
    writeHuffmanTablesFromDecoder(output, chrominanceDCBITS, chrominanceDCHuffmanValues, chrominanceACBITS, chrominanceACHuffmanValues, false);
    
    // SOS:
    emit_sos(output);
    encodeImageEntryPoint(luminanceZigZagArray, chrominanceCbZigZagArray, chrominanceCrZigZagArray, output);
    
    writeEOFMarker(output);
    
    
    cout << "Encoder Done!!" << endl;
    return true;
} // end savePicture