#include "ProtontherapyDicomAsciiReader.hh"
#include <vector> 
#include <ofstream>

using namespace std;

ProtontherapyDicomAsciiReader::ProtontherapyDicomAsciiReader()
{
	baseDirectory = "LungRT01_ascii";
	ReadToMemory();
}

ProtontherapyDicomAsciiReader::~ProtontherapyDicomAsciiReader() 
{

}

void ProtontherapyDicomAsciiReader::ReadToMemory(){ 

}