#include <fstream>
#include <list>
#include <iostream>
#include "stdio.h"
#include "dict.h"
#include "comp.h"


#define INPUT_FILE "LZBR"
//#define INPUT_FILE "test.txt"
#define OUTPUT_FILE "COMP_"
#define DECOMP_OUT "DECOMP_"

#define FILE_LEN 32768
//#define ARBDATA 


int main(){
	unsigned char rBytes[NUM_RESERVED] = R_BYTES;

#ifdef ARBDATA
	std::ofstream outTest(INPUT_FILE, std::ofstream::binary);

	for (int i = 0; i < FILE_LEN; i++) {
		outTest << (unsigned char)(rand() % 256);
	}
	outTest.close();
#endif
	std::ifstream input(INPUT_FILE, std::ifstream::binary);
	std::ofstream output(OUTPUT_FILE, std::ofstream::binary);
	compress(input, output, rBytes);
	input.close();
	output.close();

	std::ifstream ifs;
	std::ofstream ofs;
	//Read in the File
	ifs.open(OUTPUT_FILE, std::ifstream::binary);
	ofs.open(DECOMP_OUT, std::ifstream::binary);
	decomp(ifs, ofs, rBytes);
	ofs.close();
 	ifs.close();

}


