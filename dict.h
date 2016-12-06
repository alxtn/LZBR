#ifndef DICT_H
#define DICT_H

#include <list>
#include <map>
#include <fstream>
#include "string.h"

#define OFFSET1_BITS 11
#define OFFSET2_BITS (16-OFFSET1_BITS)
#define CHAR_TOKEN_LEN 8
#define KEYVAL_LEN 3
#define MAX_LINK 32

//Classes and Structs
typedef struct token {
	token(bool isMatch, char* data, unsigned int length, unsigned int offset1, unsigned int offset2);
	bool isMatch;
	unsigned int length;
	unsigned int offset1;
	unsigned int offset2;
	char data[CHAR_TOKEN_LEN];
} token;

class KeyVal{
public:
	KeyVal(char* str);
	~KeyVal();
	char operator[](int index) const;
	char* data;
private:
	
	int length;
};

struct compare
{
	bool operator() (const KeyVal* lhs, const KeyVal* rhs) const {
		return memcmp(lhs->data, rhs->data, KEYVAL_LEN) < 0;
	}
};

//Typedefs
typedef std::pair<KeyVal*, std::list<int>*> mapItem;
typedef std::map<KeyVal*, std::list<int>*,compare> myMap;

//Other Functions
void dictionary(std::ifstream& input, std::list<token*>& tokens, bool verbose);
void prune(myMap* map, int cutoff); //Cutoff is last valid index
std::ostream& operator<<(std::ostream& os, token& tok);
int countBytes(std::list<token*>* tokens);
int numBytesPresent(char* data, int length);
int findLongestMatch(char* data, int minIndex, char* lookAhead, int length,
		     std::list<int>& matches, int* matchLength); //Returns index of best match

#endif
