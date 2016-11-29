#include <iostream>
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "dict.h"

#define STRAIGHT_OUT false

void dictionary(std::ifstream& input, std::list<token*>& tokens) {
	int sBufLen = exp2(OFFSET1_BITS); //Search Buffer length
	int nBufLen = exp2(OFFSET2_BITS); //next buffer length (after current look-ahead buffer)
	int currBufLen = nBufLen;		  //Size of the look-ahead buffer (characters to be matched)
	int logBufSize = sBufLen + nBufLen + currBufLen; //Size of the logical buffer
	myMap* strings = new myMap;
	myMap* strings2 = new myMap;
	
	//Read in the File
	input.seekg(0,input.end);
	int bufferSize = input.tellg();
	input.seekg(0,input.beg);
	char* buffer = (char*)malloc(sizeof(char)*bufferSize);
	input.read(buffer, bufferSize);
	input.close();
	//printf("File contents:\n\n%s", buffer);
	std::cout << "Bytes Present:" << numBytesPresent(buffer, bufferSize) << "/256" << std::endl;
	token* thisToken = NULL;

	token* last3Tok = NULL;
	int distTo3 = 0;
	token* last4Tok = NULL;
	int distTo4 = 0;

	int i;
	for (i = 0; i < bufferSize-2; i++) {
		if (i != 0 && (i % sBufLen) == 0) {
			prune(strings, i-sBufLen);
		}

		//Generate a new Key at current position and attempt to insert
		KeyVal* key = new KeyVal(buffer + i);
		std::pair<myMap::iterator,bool> retVal = strings->insert(mapItem(key, NULL));
		if (retVal.second || STRAIGHT_OUT) { //If it didn't exist, fill in the list field and fill token
			(retVal.first)->second = new std::list<int>(1, i);
			if (!thisToken) {
				thisToken = new token(false, buffer + i, 1, 0,0);
			} else if (thisToken->length < CHAR_TOKEN_LEN) {
				thisToken->data[thisToken->length++] = buffer[i];
			} else {
				tokens.push_back(thisToken);
				thisToken = new token(false, buffer + i, 1, 0,0);
			}
			distTo3++;
			distTo4++;
		} else { //If it did exist, add this index and then search for longest match
			int matchIndex;
			int matchLength;
			delete key;
			matchIndex = findLongestMatch(buffer, i-sBufLen, buffer + i, currBufLen, *(retVal.first->second), &matchLength);
			
			if (matchIndex != -1) {
				if (thisToken) {
					tokens.push_back(thisToken);
				}
				thisToken = new token(true, NULL, matchLength, i - matchIndex, MAX_LINK - 1);
				if (matchLength == 3) {
					if (last3Tok) {
						if (distTo3 < MAX_LINK - 1) {
							last3Tok->offset2 = distTo3;
							distTo4 += 2;
						} else {
							distTo4 += 3;
						}
					} else {
						distTo4 += 3;
					}
					last3Tok = thisToken;
					distTo3 = 0;
				} else if (matchLength == 4) {
					if (last4Tok) {
						if (distTo4 < MAX_LINK - 1) {
							last4Tok->offset2 = distTo4;
							distTo3 += 2;
						} else {
							distTo3 += 3;
						}
					} else {
						distTo3 += 3;
					}
					last4Tok = thisToken;
					distTo4 = 0;
				} else {
					distTo3 += 3;
					distTo4 += 3;
				}
				tokens.push_back(thisToken);
				thisToken = NULL;
				(retVal.first)->second->push_front(i);
				i += matchLength - 1;
			} else {
				if (!thisToken) {
					thisToken = new token(false, buffer + i, 1, 0, 0);
				} else if (thisToken->length < CHAR_TOKEN_LEN) {
					thisToken->data[thisToken->length++] = buffer[i];
				} else {
					tokens.push_back(thisToken);
					thisToken = new token(false, buffer + i, 1, 0, 0);
				}
				(retVal.first)->second->push_front(i);
				distTo3++;
				distTo4++;
			}
		}
	}
	if (thisToken) {
		tokens.push_back(thisToken);
	}
	//Not valid if match was last
	if (i < bufferSize) {
		thisToken = new token(false, buffer + i, bufferSize-i, 0, 0);
		tokens.push_back(thisToken);
	}
	free(buffer);
	delete strings;
	delete strings2;
}


KeyVal::KeyVal(char* str) : length(KEYVAL_LEN) {
	data = (char*)malloc(sizeof(char)*length);
	for (int i = 0; i < length; i++) {
		data[i] = str[i];
	}
}

KeyVal::~KeyVal() {
	free(data);
}

char KeyVal::operator[](int index) const {
	if (index < length) {
		return data[index];
	} else {
		printf("KeyVal: Bad Index Attempt\n");
		return NULL;
	}
}

token::token(bool isMatch, char* input, unsigned int length, unsigned int offset1, unsigned int offset2) :
			isMatch(isMatch), length(length), offset1(offset1), offset2(offset2){
	if (input) {
		for (int i = 0; i < length; i++) {
			data[i] = input[i];
		}
	}
}

void prune(myMap* map, int cutoff) {
	for (myMap::iterator itr = map->begin(); itr != map->end(); itr++) {
		for (std::list<int>::iterator itr2 = itr->second->begin(); itr2 != itr->second->end(); itr2++) {
			if (*itr2 < cutoff) {
				itr2 = itr->second->erase(itr2);
				if (itr2 == itr->second->end()) {
					break;
				}
			}
		}
		if (itr->second->empty()) {
			itr = map->erase(itr);
			if (itr == map->end())
				break;
		}
	}
}

int findLongestMatch(char* data, int minIndex, char* lookAhead, int length, std::list<int>& matches, int* matchLength) {
	int maxIndex = -1;
	int maxLength = -1;
	if (minIndex == 340) {
		int asd = 213;
	}
	for (std::list<int>::iterator itr = matches.begin(); itr != matches.end(); itr++) {
		if (*itr <= minIndex) {
			itr = matches.erase(itr);
			if (itr == matches.end()) {
				break;
			}
		} else {
			int j;
			for (j = KEYVAL_LEN; j < length; j++) {
				if ((data[(*itr) + j] != lookAhead[j]) || j >= MAX_LINK-1) {
					break;
				}
			}
			maxIndex = j > maxLength ? *itr : maxIndex;
			maxLength = j > maxLength ? j : maxLength;
		}
	}
	if (maxIndex == -1) {
		int asd = 230;
	}
	*matchLength = maxLength;
	return maxIndex;
}

std::ostream& operator<<(std::ostream& os, token& tok) {
	char output[512];
	if (tok.isMatch) {
		sprintf(output, "{O:%d  L:%d  O2:%d}", tok.offset1, tok.length, tok.offset2);
	} else {
		memcpy(output, tok.data, tok.length);
		output[tok.length] = NULL;
	}
	os << output;
	return os;
}

int countBytes(std::list<token*>* tokens) {
	int count = 0;
	int numTokens = 0;
	for (std::list<token*>::iterator itr = tokens->begin(); itr != tokens->end(); itr++) {
		if (!(*itr)->isMatch) {
			count += (*itr)->length;
		} else {
			count += 3;
		}
		numTokens++;
	}
	return count + numTokens/8;
}

int numBytesPresent(char* data, int length) {
	unsigned char currByte;
	int count = 0;
	for (currByte = 0; currByte < 255; currByte++) {
		for (int i = 0; i < length; i++) {
			if ((unsigned char)data[i] == currByte) {
				count++;
				break;
			}
		}
	}
	return count;
}



