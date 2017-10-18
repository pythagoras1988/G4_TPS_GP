****************************************************************************
*	This code is developed to simulate proton beam therapy to the greatest
*	details for research and operation uses which does not demand real-time 
*	capability. 
*	
*	Author: Tan Hong Qi, Phua Jun Hao, Lloyd Tan, Ang Khong Wei
*	Affiliations: NUS, CIBA, NCCS
****************************************************************************


1. Features: 

1) Realistic Hitachi Beamline components with control over phase space parameters
2) Proton beam spot scanning with different weights given in a particular format 		
3) Can include DICOM data/cubic phantom as detector for dose scoring
4) The material construction is obtained from the HU data following Schneider's method
5) Can score Dose Averaged LET in phantom (developing)

2. Activate Dicom Data mode 

a) Store the raw binary DICOM data in a folder(LungRT) and run DicomHandler.py script with the folder name as the base directory. This will convert the binary data into ASCII data in a new folder(LungRT_ascii) and generate a specsFile.txt. The specsFile.txt and DICOM data in ASCII format are important input for the ProtonTherapyG4 program. Make Sure these files are present!!!! 

b) Remember to set the directory for the DICOM data in the .mac file via /beamLine/SetDicomDirectory LungRT01 -----> This is very important!!! 

c)The final dose.out file can be superposed with all the Dicom data using ProcessDoseWithCT.py file. 

3. Scanning mode in the program

a) 



