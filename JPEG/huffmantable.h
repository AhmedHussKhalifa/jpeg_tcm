//
//  huffmantable.hpp
//  TCM
//
//  Created by Hossam Amer on 2018-08-09.
//  Author: Hossam Amer & Yanbing Jiang, University of Waterloo
//  Copyright © 2018 All rights reserved.
//

#ifndef HUFFMANTABLE_H
#define HUFFMANTABLE_H

#include <map>
#include <vector>
#include "TypeDef.h"

using namespace std;

class HuffmanTable {
    
public:
    
    // ID for which Huffman table it is using (there are 32 possible Huffman tables in JPEG)
    unsigned char tableID;
    
    // table length from bitstream
    unsigned short tableSegmentLengthFromBitstream;
    
    // tableClass 0 is for DC, 1 is for AC
    unsigned char  tableClass;
    vector <unsigned int> codes;
    vector <unsigned int> codeLengths;
    
    vector <unsigned char> number_of_codes_for_each_1to16;
    
    // The array of Huffman maps: (length, code) -> value
    std::map<huffKey, unsigned char> huffData;
    
    HuffmanTable();
};

#endif // HUFFMANTABLE_H

//myfile << dataReshapedInto8x8[1][0] << ",";//1
//myfile << dataReshapedInto8x8[0][1] << ",";//2
//myfile << dataReshapedInto8x8[0][2] << ",";//3
//myfile << dataReshapedInto8x8[1][1] << ",";//4
//myfile << dataReshapedInto8x8[2][0] << ",";//5
//myfile << dataReshapedInto8x8[3][0] << ",";//6
//myfile << dataReshapedInto8x8[1][2] << ",";//7
//myfile << dataReshapedInto8x8[2][1] << ",";//8
//myfile << dataReshapedInto8x8[0][3] << ",";//9
//myfile << dataReshapedInto8x8[0][4] << ",";//10
//myfile << dataReshapedInto8x8[1][3] << ",";//11
//myfile << dataReshapedInto8x8[2][2] << ",";//12
//myfile << dataReshapedInto8x8[3][1] << ",";//13
//myfile << dataReshapedInto8x8[4][0] << ",";//14
//myfile << dataReshapedInto8x8[5][0] << ",";//15
//myfile << dataReshapedInto8x8[4][1] << ",";//16
//myfile << dataReshapedInto8x8[3][2] << ",";//17
//myfile << dataReshapedInto8x8[2][3] << ",";//18
//myfile << dataReshapedInto8x8[1][4] << ",";//19
//myfile << dataReshapedInto8x8[0][5] << ",";//20
//myfile << dataReshapedInto8x8[0][6] << ",";//21
//myfile << dataReshapedInto8x8[1][5] << ",";//22
//myfile << dataReshapedInto8x8[2][4] << ",";//23
//myfile << dataReshapedInto8x8[3][3] << ",";//24
//myfile << dataReshapedInto8x8[4][2] << ",";//25
//myfile << dataReshapedInto8x8[5][1] << ",";//26
//myfile << dataReshapedInto8x8[6][0] << ",";//27
//myfile << dataReshapedInto8x8[7][0] << ",";//28
//myfile << dataReshapedInto8x8[6][1] << ",";//29
//myfile << dataReshapedInto8x8[5][2] << ",";//30
//myfile << dataReshapedInto8x8[4][3] << ",";//31
//myfile << dataReshapedInto8x8[3][4] << ",";//32
//myfile << dataReshapedInto8x8[2][5] << ",";//33
//myfile << dataReshapedInto8x8[1][6] << ",";//34
//myfile << dataReshapedInto8x8[0][7] << ",";//35
//myfile << dataReshapedInto8x8[1][7] << ",";//36
//myfile << dataReshapedInto8x8[2][6] << ",";//37
//myfile << dataReshapedInto8x8[3][5] << ",";//38
//myfile << dataReshapedInto8x8[4][4] << ",";//39
//myfile << dataReshapedInto8x8[5][3] << ",";//40
//myfile << dataReshapedInto8x8[6][2] << ",";//41
//myfile << dataReshapedInto8x8[7][1] << ",";//42
//myfile << dataReshapedInto8x8[7][2] << ",";//43
//myfile << dataReshapedInto8x8[6][3] << ",";//44
//myfile << dataReshapedInto8x8[5][4] << ",";//45
//myfile << dataReshapedInto8x8[4][5] << ",";//46
//myfile << dataReshapedInto8x8[3][6] << ",";//47
//myfile << dataReshapedInto8x8[2][7] << ",";//48
//myfile << dataReshapedInto8x8[3][7] << ",";//49
//myfile << dataReshapedInto8x8[4][6] << ",";//40
//myfile << dataReshapedInto8x8[5][5] << ",";//51
//myfile << dataReshapedInto8x8[6][4] << ",";//52
//myfile << dataReshapedInto8x8[7][3] << ",";//53
//myfile << dataReshapedInto8x8[7][4] << ",";//54
//myfile << dataReshapedInto8x8[6][5] << ",";//55
//myfile << dataReshapedInto8x8[5][6] << ",";//56
//myfile << dataReshapedInto8x8[4][7] << ",";//57
//myfile << dataReshapedInto8x8[5][7] << ",";//58
//myfile << dataReshapedInto8x8[6][6] << ",";//59
//myfile << dataReshapedInto8x8[7][5] << ",";//60
//myfile << dataReshapedInto8x8[7][6] << ",";//61
//myfile << dataReshapedInto8x8[6][7] << ",";//62
//myfile << dataReshapedInto8x8[7][7] << ",";//623