# JPEG with Transparent Composite Model (TCM)
Authors: Hossam Amer (@HossamAmer12) and Yanbing Jiang (@YanbingJiang)

Created: September 2018

### Content Included
- JPEG Baseline Model Decoder and Encoder with Implementation of TCM

### Prerequisites
- Enviornment: C++
- IDE: Visual Studio 2017 (Windows), XCode (MAC OS)

### Updates
- 2018-09-14: First Upload, Sequential Model Done (0xFFC0)
- 2018-09-17: Progressive: Fifth Scan Done for goose_org.jpeg, Huffman Table overwritten, Dequantiz not done
- 2018-09-20: Progressive: All Scan Done for goose_org.jpeg, Huffman Table overwritten, Dequantiz not done
- 2018-09-23: Progressive: All Scan done, Data Read properly, but one tcoeff reason unknown, Huffman Table overwritten
- 2018-09-24: Tested on Multiple SOI (0xFFD8): Success, Progressive Decoder Clean up, Y Component Shown in Release Mode of VS2017 but not Debug Mode
- 2018-09-25: RGB-BMP Picture shown bug fixed
- 2018-09-26: Progressive Encoder Standard Default Huffman Table Built
- 2018-09-26: Support multiple compilation modes and added code for Huffman tables and SOF writing in the progressive mode (trying to write as sequential)
- 2018-09-27: Progressive Encoder as Sequential with Standard Default Huffman Table Built Done
- 2018-09-28: Support Multiple picture processing (decode and encode), utilize with flag "MULTI_PIC"
- 2018-10-01: Multion SOI (0xFFD8) bug found and fixed, not dog.JPEG/lion_org.jpg can be processed
- 2018-10-10: Support adjust Picture with Quality Factor (var QFACTOR in Typedef.h, not tested due to VS2017 issue)
- 2018-10-15: Default huffman table writing is done to QF Experiement and Same QTable for QTable experiment is done
- 2018-10-16: QF Experiment Configuration for VS2017 is done using default QT and HT as standard
- 2018-10-17: Rounding value has to exist in Quantization process and Fix default HT
- 2018-10-20: QF Experiment Configuration for dataset finalized and fix BW picture bug 
- 2018-10-25: Added Inception V3 Group Running Python Sheet and MATLAB analysis sheet
- 2018-11-04:  Optimised the JPEG decoder and the encoder to run 4k Pictures in about 20 seconds
