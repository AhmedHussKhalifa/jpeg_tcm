//
//  TypeDef.h
//  TCM
//
//  Created by Hossam Amer on 2018-08-09.
//  Author: Hossam Amer & Yanbing Jiang, University of Waterloo
//  Copyright © 2018 All rights reserved.
//

#include "stdafx.h"

#ifndef TYPEDEF_H
#define TYPEDEF_H

#define PRINT_HUFFMAN_TABLE              0
#define PRINT_QUANTIZATION_TABLE         0
#define PRINT_FRAME_HEADER_SOF           1
#define PRINT_SOS                        1
#define PRINT_BLOCK_PROGRESS             0
#define DEBUGLEVEL                       12

#define OPEN_CV_ENABLED                  1
#define DISPLAY_BMP_IMAGE                1

// Component indices
#define COMPONENT_Y                      0
#define COMPONENT_Cb                     1
#define COMPONENT_Cr                     2

// Table length for writing two quantization tables back to back with two DQT markers
#define DQT_LENGTH_WITH_TWO_MARKERS      67 


// Table length for writing two quantization tables back to back with one DQT marker
#define DQT_LENGTH_WITH_ONE_MARKERS      132

// JPEG Buffer maximum output size to write sections of the bitstream instead of byte by byte
#define JPEG_OUT_HEADER_SIZE			 500

// Is Fast way of writing in the encoder?
#define IS_JPEG_ENCODER_WRITE_FAST       0

// Threshold that flag of the count block outlier to to be non-black block
#define NON_BLACK						 2


#define IS_ONLY_TCM                       1
#define TCM_OUTLIER_THRESHOLD             8



// Defines a tuple of length and code, for use in the Huffman maps
typedef std::pair<int, unsigned short> huffKey;


#endif /* TypeDef_h */
