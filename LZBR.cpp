#include <fstream>
#include <list>
#include <iostream>
#include "stdio.h"
#include "dict.h"
#include "comp.h"
#include "string.h"

int main(int argc, char **argv){

  unsigned char rBytes[NUM_RESERVED] = R_BYTES;
   
  if(argc != 4) //Currently supporting one file at a time
    std::cout << "Usage: LZBR [mode] <input-file> <ouput-file>" << std::endl;

  char* inputFile = argv[2];
  char* outputFile = argv[3];
  std::ifstream ifs(inputFile, std::ifstream::binary);
  std::ofstream ofs(outputFile, std::ifstream::binary);

  if(!strcmp("-c", argv[1])){ //Compressing
    compress(ifs, ofs, rBytes);
    ofs.close();
    ifs.close();
  } else if(!strcmp("-d", argv[1])){ //Decompressing
    decomp(ifs, ofs, rBytes);
    ofs.close();
    ifs.close();
  } else {
    std::cout << "Use a valid mode: -c or -d" << std::endl;
  }
}


