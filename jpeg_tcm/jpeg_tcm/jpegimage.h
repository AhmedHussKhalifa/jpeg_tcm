//
//  jpegimage.hpp
//  TCM
//
//  Created by Hossam Amer on 2018-08-09.
//  Author: Hossam Amer & Yanbing Jiang, University of Waterloo
//  Copyright © 2018 All rights reserved.
//

#ifndef JPEGIMAGE_H
#define JPEGIMAGE_H

#include "inttypes.h"
#include <string>
#include <vector>
#include <map>
#include <stdio.h>

// Macro to read a 16-bit word from file
#define READ_WORD() ((fgetc(fp) << 8) | fgetc(fp))

// Segment parsing error codes
#define JPEG_SEG_ERR  0
#define JPEG_SEG_OK   1
#define JPEG_SEG_EOF -1

class jpeg_image {
private:
    // Names of the possible segments
    std::string segNames[64];
    
    // The file to be read from, opened by constructor
    FILE *fp;
    
    // the input file name
    std::string jpeg_filename;
    
    // Segment parsing dispatcher
    int parseSeg();
    
public:
    // Construct a JPEG object given a filename
    jpeg_image(std::string);
};


#endif /* JPEGIMAGE_H */
