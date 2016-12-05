#ifndef COMP_H
#define COMP_H

#include <list>
#include <fstream>

//Not linked to offset packing
#define BLOCK_BUFFER_SIZE 2048
#define NUM_RESERVED_BYTES 3

#define BLOCK_SIZE 250
#define NUM_BYTES 256
#define NUM_BITS 8
#define NUM_RESERVED 3
#define R_BYTES {(unsigned char)'q', (unsigned char)'t', (unsigned char)'u'}

void compress(std::ifstream& input, std::ofstream& output,
	      unsigned char* rBytes, bool verbose);
void decomp(std::ifstream& input, std::ofstream& output, unsigned char* rBytes,
	    bool verbose);
void rByteEncode(std::list<token*>& tokens, std::ofstream& output,
		 unsigned char* rBytes, int length, bool verbose);
int getBlock(std::list<token*>::iterator itr, std::list<token*>::iterator& endItr,
	     unsigned char* dest, int blockLen, std::list<token*>::iterator end,
	     bool& endFlag, int& numMatches);
bool processBlock(unsigned char* block, int length, unsigned char* rBytes,
		  int numBytes, unsigned char* solution);
int getPresentBytes(unsigned char* data, int length, int* arr);
void writeBlock(unsigned char* block, int length, unsigned char* rBytes, int numBytes,
		unsigned char solution, std::list<token*>::iterator start,
		std::list<token*>::iterator end, std::ofstream& output);
void packOffsets(int offset1, int offset2, unsigned char* output);
void outBinary(unsigned char in);

#endif
