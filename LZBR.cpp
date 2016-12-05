#include <fstream>
#include <list>
#include <iostream>
#include "stdio.h"
#include "dict.h"
#include "comp.h"
#include "string.h"

#define DEFAULT_EXTENSION ".lz"

int main(int argc, char **argv){

	unsigned char rBytes[NUM_RESERVED] = R_BYTES;
	bool verbose = false;
	unsigned int modeArg = 1;

	//Currently supporting one file at a time
	if(argc != 4 && argc != 5){ 
		std::cout
			<< "Usage: LZBR [-v] <mode> <input-file> <ouput-file>"
			<< std::endl;
		exit(1);
	}

	if(!strcmp("-v", argv[1])){
		verbose = true;
		modeArg = 2;
	}

	char* inputFile = argv[modeArg+1];
	char* outputFile = argv[modeArg+2];
	std::ifstream ifs(inputFile, std::ifstream::binary);
	std::ofstream ofs(outputFile, std::ifstream::binary);

	if(!strcmp("-c", argv[modeArg])){ //Compressing
		compress(ifs, ofs, rBytes, verbose);
		ofs.close();
		ifs.close();
	} else if(!strcmp("-d", argv[modeArg])){ //Decompressing
		decomp(ifs, ofs, rBytes, verbose);
		ofs.close();
		ifs.close();
	} else {
		std::cout << "Use a valid mode: -c or -d" << std::endl;
	}
}


