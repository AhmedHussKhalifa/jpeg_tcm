// August 28 trial
//bool jpeg_encoder::savePicture() {
//    
//    cout << "\n\n Encode Start " << endl;
//    
//    // No need to transform
//    //    transformToYCbCr();
//    
//    // Copy the quantization tables
//    copy_qTables();
//    
//    
//    //In these arrays I will keep my data after FDCT
//    vector<int> luminanceZigZagArray;
//    vector<int> chrominanceCbZigZagArray;
//    vector<int> chrominanceCrZigZagArray;
//    
//    
//    //Prosess every component with FDCT
//    uint_8 ** luminance     = jpegDecoder->m_YPicture_buffer;
//    uint_8 ** chrominanceCb = jpegDecoder->m_CbPicture_buffer;
//    uint_8 ** chrominanceCr = jpegDecoder->m_CrPicture_buffer;
//    
//    
//    perform_fdct(luminance, luminanceZigZagArray, quantization_table_write_process_luminance.quantizationTableData, COMPONENT_Y);
//    perform_fdct(chrominanceCb, chrominanceCbZigZagArray, quantization_table_write_process_chrominance.quantizationTableData, COMPONENT_Cb);
//    perform_fdct(chrominanceCr, chrominanceCrZigZagArray, quantization_table_write_process_chrominance.quantizationTableData, COMPONENT_Cr);
//    
//    // TODO: delete the luminance and chrominance -- free up the decoder
//    // We don't need data in matrix anymore, all data is saved in zigZag arrays previously declared in this function
//    
//    
//    //Now we have to prepare things for Huffman coding
//    
//    /*
//     *   Every DC coefficient is calculated as Difference = DCi - DCi-1
//     *   After that, we calculate category of Difference with getCategoryOfDCTCoefficient() function
//     *   Category is 8 bits long.
//     *   After category, we calculate Difference element bit representation. If Difference is < 0 then bit representation is Difference -2^category+1, else bit representation is Difference value.
//     *
//     */
//    
//    vector <int>luminanceDCElementsFrequency(257,0); // Max category of DC element is 15
//    vector <int> luminanceACElementsFrequency(257,0); // Max value of 8 bits is 255 so we need vector with 256 elements
//    luminanceDCElementsFrequency[256] = 1;
//    luminanceACElementsFrequency[256] = 1;
//    
//    vector<char> luminanceCategoryByte(0);// Here I will save category of every element
//    vector<short int>luminanceValues(0); // Here I will save bit representation of every value
//    
//    
//    vector <int>chrominanceDCElementsFrequency(257,0); // DC elements frequency for Cb and Cr componenet
//    vector <int> chrominanceACElementsFrequency(257,0); // AC elements frequency for Cb and Cr component
//    chrominanceDCElementsFrequency[256] = 1;
//    chrominanceACElementsFrequency[256] = 1;
//    
//    vector<char> chrominanceCbCategoryByte(0);
//    vector<short int>chrominanceCbValues(0);
//    
//    vector<char> chrominanceCrCategoryByte(0);
//    vector<short int>chrominanceCrValues(0);
//    
//    // TODO: remove fankoosh
//    // cut the luminance zigzagarray
////    luminanceZigZagArray.erase(luminanceZigZagArray.begin() + 64*2, luminanceZigZagArray.end());
////    luminanceZigZagArray.erase(luminanceZigZagArray.begin() + 64, luminanceZigZagArray.end());
//    
//   
//    
//    
//    // Generate frequencies:
//    generateCategoryFrequences(luminanceZigZagArray, luminanceDCElementsFrequency,
//                               luminanceACElementsFrequency, luminanceCategoryByte, luminanceValues);
//
//    generateCategoryFrequences(chrominanceCbZigZagArray, chrominanceDCElementsFrequency,
//                               chrominanceACElementsFrequency, chrominanceCbCategoryByte, chrominanceCbValues);
//    generateCategoryFrequences(chrominanceCrZigZagArray, chrominanceDCElementsFrequency,
//                               chrominanceACElementsFrequency, chrominanceCrCategoryByte,chrominanceCrValues);
//    
//    // After this step, we can delete ZigZagArrays of all three components because all data needed is now stored in vectors passed to function generateCategoryFrequences();
//    luminanceZigZagArray.clear();
//    chrominanceCbZigZagArray.clear();
//    chrominanceCrZigZagArray.clear();
//    
//    vector<int>luminanceDCCodeLenghts(257,0);
//    vector<int>luminanceACCodeLenghts(257,0);
//    vector<char>luminanceDCBITS(33,0);
//    vector<char>luminanceACBITS(33,0);
//    vector<int>luminanceDCOthers(257,-1);
//    vector<int>luminanceACOthers(257,-1);
//    vector<int>luminanceDChuffmanValues;
//    vector<int>luminanceAChuffmanValues;
//    
//    vector<int>chrominanceDCCodeLengths(257,0);
//    vector<int>chrominanceACCodeLengths(257,0);
//    vector<int>chrominanceDCOthers(257,-1);
//    vector<int>chrominanceACOthers(257,-1);
//    vector<char>chrominanceDCBITS(33,0);
//    vector<char>chrominanceACBITS(33,0);
//    vector<int>chrominanceDCHuffmanValues;
//    vector<int>chrominanceACHuffmanValues;
//    
//    
//    vector<int> luminanceCodesDC(256,-1);
//    vector<int> luminanceCodesAC(256,-1);
//    vector<int>luminanceLengthsDC(256,-1);
//    vector<int> luminanceLengthsAC(256,-1);
//    
//    vector<int> chrominanceCodesDC(256,-1);
//    vector<int>chrominanceCodesAC(256,-1);
//    vector<int> chrominanceLengthsDC(256,-1);
//    vector<int>chrominanceLengthsAC(256,-1);
//    
//    
//    // Counting for luminance DC elements
//    
//    // Once the code lengths for each symbol have been obtained
//    generateCodeSizes(luminanceDCElementsFrequency, luminanceDCCodeLenghts, luminanceDCOthers);
//    
//    countNumbersOfCodesCodedWithKBits(luminanceDCBITS, luminanceDCCodeLenghts); // where k is something between 0 to 255
//    adjustBitLengthTo16Bits(luminanceDCBITS); // change the code sizes from 0 to 15 now
//    generateBytesAfterBITS(luminanceDCCodeLenghts, luminanceDChuffmanValues);
//    
//    // (vector<char>&BITS, vector<int>bytes, vector<int>&byteHuffmanCode, vector<int>&byteHuffmanCodeLength)
//    generateHuffmanCodes(luminanceDCBITS, luminanceDChuffmanValues, luminanceCodesDC, luminanceLengthsDC);
//    
//    
//    // TODO: remove this
////    cout << "Codes Size: " << luminanceCodesDC.size() << endl;
////    cout << "Codes Length Size: " << luminanceLengthsDC.size() << endl;
////    
////    cout << "Remove Fankoosh " << endl;
////    for(int i = 0; i < luminanceCodesDC.size(); ++i)
////    {
////        cout << "[" << luminanceCodesDC.at(i) << ", " << luminanceLengthsDC.at(i) << "], " ;
////    }
////    
////    
////    cout << "Remove Fankoosh " << endl;
////    for(int i = 0; i < luminanceDCBITS.size(); ++i)
////    {
////        cout << "[" << int(luminanceDCBITS.at(i)) << "], " ;
////    }
////    exit(0);
//    
//    // Counting for luminance AC elements
//    generateCodeSizes(luminanceACElementsFrequency, luminanceACCodeLenghts, luminanceACOthers);
//    countNumbersOfCodesCodedWithKBits(luminanceACBITS, luminanceACCodeLenghts);
//    adjustBitLengthTo16Bits(luminanceACBITS);
//    generateBytesAfterBITS(luminanceACCodeLenghts, luminanceAChuffmanValues);
//    generateHuffmanCodes(luminanceACBITS, luminanceAChuffmanValues, luminanceCodesAC, luminanceLengthsAC); // Number of codes coded with 1,2,3,4....,16 bits are saved in chromianceBITS vector. Concrete values are saved in chrominance(luminance)AC(DC)huffmanValues. Huffman codes for elements is saved in chrominance(luminance)codesAC(DC) and code lengths is stored in chrominance(luminance)AC(DC)CodeLenthts
//    
//    
//    // Counting for chrominance DC elements
//    generateCodeSizes(chrominanceDCElementsFrequency, chrominanceDCCodeLengths, chrominanceDCOthers);
//    countNumbersOfCodesCodedWithKBits(chrominanceDCBITS, chrominanceDCCodeLengths);
//    adjustBitLengthTo16Bits(chrominanceDCBITS);
//    generateBytesAfterBITS(chrominanceDCCodeLengths, chrominanceDCHuffmanValues);
//    generateHuffmanCodes(chrominanceDCBITS, chrominanceDCHuffmanValues, chrominanceCodesDC, chrominanceLengthsDC);//Number of codes coded with 1,2,3,4....,16 bits are saved in chromianceBITS vector. Concrete values are saved in chrominance(luminance)AC(DC)huffmanValues. Huffman codes for elements is saved in chrominance(luminance)codesAC(DC) and code lengths is stored in chrominance(luminance)AC(DC)CodeLenthts
//    
//    // Counting for chrominance AC elements
//    generateCodeSizes(chrominanceACElementsFrequency, chrominanceACCodeLengths, chrominanceACOthers);
//    countNumbersOfCodesCodedWithKBits(chrominanceACBITS, chrominanceACCodeLengths);
//    adjustBitLengthTo16Bits(chrominanceACBITS);
//    generateBytesAfterBITS(chrominanceACCodeLengths, chrominanceACHuffmanValues);
//    generateHuffmanCodes(chrominanceACBITS, chrominanceACHuffmanValues, chrominanceCodesAC, chrominanceLengthsAC);//Number of codes coded with 1,2,3,4....,16 bits are saved in chromianceBITS vector. Concrete values are saved in chrominance(luminance)AC(DC)huffmanValues. Huffman codes for elements is saved in chrominance(luminance)codesAC(DC) and code lengths is stored in chrominance(luminance)AC(DC)CodeLenthts
//    
//    
//    /*
//     *   In jpeg file, there is no huffman table. Rather there is stream of bits like this:
//     *    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 - Number of bits (this information is not coded in file, but I write it here for understanding reason)
//     *   00 00 05 06 00 00 00 01 00 00 00 03 00 00 00 01 after this stream of beats elements starts 02 03 04 05 06 07 08 09 A0 B0
//     *   This means that there are 0 elements coded with 00 bits, 00 elements coded with 1 bit
//     *   05 elements coded with 2 bits, 06 elements coded with 3 bits etc....
//     *   Elements coded with 2 bits are: 02 03 04 05 06
//     *   Elements coded with 3 bits are: 07 08 09 A0 B0 ....
//     *   So in my huffman table we have to know how many elements I have coded with 0,1,2,3,4 etc.. bits
//     *   For that reason I developed class HuffmanElementsCount.h In argument codeLength I keep code length, and in vector
//     *   elementsCodedWithCodeLengthBits I keep elements(symbols). This information I need to write in file.
//     */
//    
//    // Here starts file writing process:
//    string fileName = image_to_export_filename;
//    ofstream output(fileName.c_str(), ios::out | ios::binary);
//    
//    // emit start markers
//    emit_start_markers(output);
//    
//    // Write huffman table for luminance component
////    writeHuffmanTables(output, luminanceDCBITS, luminanceDChuffmanValues, luminanceACBITS, luminanceAChuffmanValues, true);
////    // Write huffman table for chrominance components
////    writeHuffmanTables(output, chrominanceDCBITS, chrominanceDCHuffmanValues, chrominanceACBITS, chrominanceACHuffmanValues, false);
//    
//    // Write decoder huffman tables:
//    writeHuffmanTablesFromDecoder(output, chrominanceDCBITS, chrominanceDCHuffmanValues, chrominanceACBITS, chrominanceACHuffmanValues, false);
//    
//    
//    // TODO: remove this
//    std::flush(output);
//    output.close();
//    exit(0);
//    
//    // Following vectors are not need anymore so we can delete its data
//    luminanceDCBITS.clear();
//    luminanceDChuffmanValues.clear();
//    luminanceACBITS.clear();
//    luminanceAChuffmanValues.clear();
//    
//    chrominanceDCBITS.clear();
//    chrominanceDCHuffmanValues.clear();
//    chrominanceACBITS.clear();
//    chrominanceACHuffmanValues.clear();
//    
//    // SOS:
//    emit_sos(output);
//    writeScanHeaderData(output, luminanceCodesDC, luminanceCodesAC,
//                        luminanceLengthsDC, luminanceLengthsAC,
//                        chrominanceCodesDC, chrominanceCodesAC,
//                        chrominanceLengthsDC,chrominanceLengthsAC,
//                        luminanceCategoryByte, luminanceValues,
//                        chrominanceCbCategoryByte, chrominanceCbValues,
//                        chrominanceCrCategoryByte, chrominanceCrValues);
//    writeEOFMarker(output);
//    
//    
//    cout << "Encoder Done!!" << endl;
//    return true;
//} // end savePicture
