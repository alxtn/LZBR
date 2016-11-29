#include "stdio.h"
#include <iostream>
#include <fstream>
#include <list>
#include "dict.h"
#include "comp.h"
//#include <intrin.h>
#include "math.h"

void compress(std::ifstream& input, std::ofstream& output, unsigned char* rBytes){
	
	// Allocate space for list, perform dictionary compression
	std::list<token*>* tokens = new std::list < token* > ;
	dictionary(input, *tokens);
	for (std::list<token*>::iterator itr = tokens->begin(); itr != tokens->end(); itr++) {
		//std::cout << **itr;
	}
	std::cout << std::endl << "Total Bytes Output: " << countBytes(tokens) << std::endl;

	rByteEncode(*tokens, output, rBytes,NUM_RESERVED);
	std::cout << "Match 3: ";
	outBinary(rBytes[0]);
	std::cout << std::endl << "Match 4: ";
	outBinary(rBytes[1]);
	std::cout << std::endl << "Match ^: ";
	outBinary(rBytes[2]);
	std::cout << std::endl;

	
	delete tokens;
}


void rByteEncode(std::list<token*>& tokens, std::ofstream& output,unsigned char* rBytes, int numBytes) {
	unsigned char block[BLOCK_SIZE];

	int numMatchBytes = 0;
	bool endFlag = false;
	std::list<token*>::iterator start, end;
	start = tokens.begin();
	while (!endFlag) {

		int length = getBlock(start, end, block, BLOCK_SIZE, tokens.end(), endFlag, numMatchBytes);

		//std::cout << "The length is: " << length << std::endl;
		for (int i = 0; i < length; i++) {
			//std::cout << block[i];
		}

		unsigned char solution;
		bool found = processBlock(block, length, rBytes, numBytes, &solution);
		if (found) {
			writeBlock(block, length+numMatchBytes, rBytes, numBytes, solution, start, end, output);
		}
		start = end;
	}
	int da = 4;
}

bool processBlock(unsigned char* block,int length, unsigned char* rBytes, int numBytes, unsigned char* solution) {
	int* present = (int*)malloc(sizeof(int)*NUM_BYTES);
	static int failedBlocks = 0;
	static int numBlocks = 0;
	for (int i = 0; i < NUM_BYTES; i++) {
		present[i] = 0;
	}
	int numMissing = getPresentBytes(block, length, present);

	unsigned char** lists = (unsigned char**)malloc(sizeof(unsigned char*)*numBytes);
	for (int i = 0; i < numBytes; i++) {
		lists[i] = (unsigned char*)malloc(sizeof(unsigned char)*numMissing);
		int index = 0;
		//std::cout << "For Byte: ";
		//outBinary(rBytes[i]);
		//std::cout << std::endl;
		for (int j = 0; j < NUM_BYTES; j++) {
			if (!present[j]) {
				//std::cout << "Missing: ";
				//outBinary((unsigned char)j);
				lists[i][index++] = rBytes[i] ^ ((unsigned char)j);
				//std::cout << " Candidate: ";
				//outBinary(lists[i][index - 1]);
				//std::cout << std::endl;
			}
		}
	}

	for (int i = 0; i < NUM_BYTES; i++) {
		present[i] = 0;
	}

	for (int i = 0; i < numBytes; i++) {
		getPresentBytes(lists[i], numMissing, present);
	}

	unsigned char key = 0;
	bool sol = false;
	for (int i = 0; i < NUM_BYTES; i++) {
		if (present[i] == numBytes) {
			key = (unsigned char)i;
			*solution = key;
			sol = true;
			break;
		}
	}
	numBlocks++;
	if (!sol) {
		//std::cout << "Failed" << std::endl;
		failedBlocks++;
		std::cout << "Failed: " << failedBlocks << " Total: " << numBlocks << std::endl;
		return false;
	} else {
		//std::cout << std::endl << "Solution: ";
		///outBinary(key);
		//std::cout << std::endl;
		std::cout << "Failed: " << failedBlocks << " Total: " << numBlocks << std::endl;

		for (int i = 0; i < length; i++) {
			block[i] ^= key;
		}
		return true;
	}
}

void writeBlock(unsigned char* block, int length, unsigned char* rBytes, int numBytes, unsigned char solution, std::list<token*>::iterator start, std::list<token*>::iterator end, std::ofstream& output) {
	int index = 0;
	bool isGood = output.good();
	unsigned char packedData[2];
	static bool linked3 = false;
	static bool linked4 = false;
	//DO LENGTH CHECK
	output << (unsigned char)solution << (unsigned char)length;
	for (std::list<token*>::iterator itr = start; itr != end; itr++) {
		int len = (*itr)->length;
		if ((*itr)->isMatch) {
			if (len == 3) {
				packOffsets((*itr)->offset1, (*itr)->offset2, packedData);
		
				if (!linked3) {
					output << rBytes[0];
				}
				output << packedData[0] << packedData[1];

				if ((*itr)->offset2 != MAX_LINK-1) {
					linked3 = true;
				} else {
					linked3 = false;
				}
			} else if (len == 4) {
				packOffsets((*itr)->offset1, (*itr)->offset2, packedData);

				if (!linked4) {
					output << rBytes[1];
				}
				output << packedData[0] << packedData[1];
				if ((*itr)->offset2 != MAX_LINK-1) {
					linked4 = true;
				} else {
					linked4 = false;
				}
			} else {
				packOffsets((*itr)->offset1, (*itr)->length, packedData); 
				output << rBytes[2] << packedData[0] << packedData[1];
			}
		} else {
			for (int i = 0; i < len; i++) {
				output << (char)block[index++];
			}
		}
	}

}

void packOffsets(int offset1, int offset2, unsigned char* output) {
	if (offset1 >(int)exp2(11.0) || offset2 >= (int)exp2(5.0)) {
		std::cout << "Offset too large: " << offset1 << " or " << offset2 << std::endl;
	}


	output[0] = (unsigned char)(255 & (offset1>>3));
	output[1] = (unsigned char)(255 & ((offset2 & 31) | ((offset1 & 7) << 5)));

}



int getBlock(std::list<token*>::iterator itr, std::list<token*>::iterator& endItr, unsigned char* dest, int blockLen, std::list<token*>::iterator end, bool& endFlag, int& numMatches) {
	
	static bool linked3 = false;
	static bool linked4 = false;
	int extra = 0;
	int index = 0;
	int count=0;
	numMatches = 0;
	while (count< blockLen) {
		int length = (*itr)->length;
		if (!(*itr)->isMatch) {
			if (length + count > blockLen) { //CHANGED FROM >= FOR EXPERIMENT
				break;
			}
			for (int j = 0; j < length; j++) {
				dest[index++] = (unsigned char)(*itr)->data[j];
				count++;
			}
		} else {
			
			if (count + 2 > blockLen || count + 3 > blockLen) {
				break;
			}
			if (length == 3) {
				if (linked3) {
					count += 2;
					extra += 2;
				} else {
					count += 3;
					extra += 3;
				}
				if ((*itr)->offset2 != MAX_LINK-1) {
					linked3 = true;
				} else {
					linked3 = false;
				}
			} else if (length == 4) {
				if (linked4) {
					count += 2;
					extra += 2;
				} else {
					count += 3;
					extra += 3;
				}
				if ((*itr)->offset2 != MAX_LINK-1) {
					linked4 = true;
				} else {
					linked4 = false;
				}
			} else {
				count += 3;
				extra += 3;
			}
		}
		itr++;
		if (itr == end) {
			endFlag = true;
			break;
		}
	}
	endItr = itr;
	numMatches = extra;
	return count - extra;

}

int getPresentBytes(unsigned char* data, int length, int* arr) {

	for (int i = 0; i < length; i++) {
		arr[data[i]]++;
	}
	int count = 0;
	for (int i = 0; i < NUM_BYTES; i++) {
		if (!arr[i])
			count++;
	}
	return count;
}

void outBinary(unsigned char in) {
	char output[NUM_BITS+1];
	for (int i = 0; i < NUM_BITS; i++) {
		unsigned char val = 128 / (int)exp2((double)i);
		output[i] = (in & val) ? '1' : '0';
	}
	output[NUM_BITS] = NULL;
	std::cout << output;
}

void decomp(std::ifstream& input, std::ofstream& output, unsigned char* rBytes) {
	input.seekg(0, input.end);
	int bufferSize = input.tellg();
	input.seekg(0, input.beg);
	char* buffer = (char*)malloc(sizeof(char)*bufferSize);
	char* outputBuffer = (char*)malloc(sizeof(char)*bufferSize*8);
	input.read(buffer, bufferSize);

	int index = 0;
	int next3 = -1;
	int next4 = -1;
	int outIndex = 0;
	int length = 0;

	int count3h = 0;
	int count4h = 0;
	int countUp = 0;
	int count3l = 0;
	int count4l = 0;



	while (true) {
		unsigned char sol = buffer[index++];
		length += (int)((unsigned char)buffer[index++]);
		length += 2;
		next3 += 2;
		next4 += 2;
		while(index < length) {
			if (index == next3) {
				count3l++;
				int offset1 = (int)((((int)((unsigned char)buffer[index])) << 3) | ((buffer[index + 1] >> 5) & 7));
				//int offset1 = (int)((buffer[index] << 3) | ((buffer[index + 1] >> 5) & 7));
				int offset2 = (int)(buffer[index + 1] & 31);
				for (int j = 0; j < 3; j++) {
					outputBuffer[outIndex + j] = outputBuffer[outIndex - offset1 + j];
				}
				outIndex += 3;
				index += 2;
				if (offset2 != MAX_LINK - 1) {
					next3 = index + offset2;
				}
			} else if (index == next4) {
				count4l++;
				int offset1 = (int)((((int)((unsigned char)buffer[index])) << 3) | ((buffer[index + 1] >> 5) & 7));
				//int offset1 = (int)((buffer[index] << 3) | ((buffer[index + 1] >> 5) & 7));
				int offset2 = (int)(buffer[index + 1] & 31);
				if (offset1 == 1177) {
					int asd = 0124;
				}
				for (int j = 0; j < 4; j++) {
					outputBuffer[outIndex + j] = outputBuffer[outIndex - offset1 + j];
				}
				outIndex += 4;
				index += 2;
				if (offset2 != MAX_LINK - 1) {
					next4 = index + offset2;
				}
			} else if ((unsigned char)buffer[index] == rBytes[0]) {
				count3h++;
				int offset1 = (int)((((int)((unsigned char)buffer[index + 1])) << 3) | ((buffer[index + 2] >> 5) & 7));
				if (offset1 == 578) {
					int asd = 0124;
				}
				//int offset1 = (int)((buffer[index + 1] << 3) | ((buffer[index + 2] >> 5) & 7));
				int offset2 = (int)(buffer[index + 2] & 31);
				for (int j = 0; j < 3; j++) {
					outputBuffer[outIndex+j] = outputBuffer[outIndex - offset1 + j];
				}
				outIndex += 3;
				index += 3;
				if (offset2 != MAX_LINK-1) {
					next3 = index + offset2;
				}
			} else if ((unsigned char)buffer[index] == rBytes[1]) {
				count4h++;
				int offset1 = (int)((((int)((unsigned char)buffer[index + 1])) << 3) | ((buffer[index + 2] >> 5) & 7));
				//int offset1 = (int)((buffer[index + 1] << 3) | ((buffer[index + 2] >> 5) & 7));
				int offset2 = (int)(buffer[index + 2] & 31);

				for (int j = 0; j < 4; j++) {
					outputBuffer[outIndex+j] = outputBuffer[outIndex - offset1 + j];
				}
				outIndex += 4;
				index += 3;
				if (offset2 != MAX_LINK-1) {
					next4 = index + offset2;
				}
			} else if (buffer[index] == rBytes[2]) {
				countUp++;
				int offset1 = (int)((((int)((unsigned char)buffer[index + 1])) << 3) | ((buffer[index + 2] >> 5) & 7));
				int length = (int)(buffer[index + 2] & 31);
				for (int j = 0; j < length; j++) {
					outputBuffer[outIndex+j] = outputBuffer[outIndex - offset1 + j];
				}

				outIndex += length;
				index += 3;

			}  else{
				outputBuffer[outIndex++] = (buffer[index++] ^ sol);
			}
		}
		if (index >= bufferSize)
			break;
	}
	for (int i = 0; i < outIndex; i++) {
		output << outputBuffer[i];
	}
	printf("Match Counts:\n3Heads: %d, 3Intern: %d\n4Heads: %d, 4Intern: %d\nLong: %d", count3h, count3l, count4h, count4l, countUp);
}





