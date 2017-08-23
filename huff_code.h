#ifndef HUFF_CODE_H
#define HUFF_CODE_H

#include <stdio.h>
#include <vector>

using namespace std;

class HuffCodeTree;

/*******************************************************************
class HuffCode is used to represent the Huffman code of a word.
__code stores the bit codes, __end indicates the terminate position.
for example,
i)	if 'a' is encoded to be a 5-bit long code, it should be
	__code:	xxxxx000
	__end:	00001000
ii)	if 'z' is encoded to be a 11-bit long code, it should be
	__code:	xxxxxxxx xxx00000
	__end:			 00100000
*******************************************************************/
class HuffCode {
friend class HuffCodeTree;
private:
	vector<unsigned char> __code;
	unsigned char __end;
public:
	HuffCode()
		: __code(vector<unsigned char>()), __end(1) {}
	HuffCode(const HuffCode& hc)
		: __code(hc.__code), __end(hc.__end) {}
	void append(bool);
	void cuttail();
#ifdef _DATA
	//for test
	void printCode(FILE* output) { fputc(__code[0], output); }
#endif
};	//class HuffCode

/*******************************************************************
class FreqWord is not only a data structure that stores a character
with its frequency, but it is also a treenode structure that stores
the addresses of its child, used to support the tree structure of
class HuffCodeTree
*******************************************************************/
class FreqWord {
friend class HuffCodeTree;
private:
	unsigned char word;
	double freq;
	int depth;
	FreqWord* __l;
	FreqWord* __r;
public:
	FreqWord(unsigned char w, double f, FreqWord* left=0, FreqWord* right=0)
		: word(w), freq(f), depth(0), __l(left), __r(right) {
		if(__l!=0) depth = __l->depth + 1;
		if(__r!=0) if(__r->depth > depth) depth = __r->depth + 1;
	}
	FreqWord(const FreqWord& fw)
		: word(fw.word), freq(fw.freq), depth(fw.depth), __l(fw.__l), __r(fw.__r) {}
	~FreqWord() {
		if(__l!=0)delete __l;
		if(__r!=0)delete __r;
	}
	bool operator<(const FreqWord& fw) const {
		return (freq!=fw.freq) ? freq < fw.freq : depth < fw.depth;
	}
	bool isLeaf() const { return __l==0 && __r==0; }
};	//class FreqWord

/********************************************************************
class HuffCodeTree provides functions that helps user to encode and
compress their files.

(constructor)
HuffCodeTree(FILE* input):
	Analyze the input file and build a best tree for this file.

(member function)
void encode(FILE* input, FILE* output):
	Compress the input file and write the result to the output file.
bool decode(FILE* input, FILE* output):
	Read the input file as if it is a compressed file, uncompress it
	and write the result to the output file. The return value is
	whether the file is in the format we expected.
********************************************************************/
class HuffCodeTree {
private:
	FreqWord* decoder;
	HuffCode* encoder;
	void clear();
	void givingCode(const FreqWord*, HuffCode&);
	void printTree(FILE*)const;
	void parseOrder(unsigned char*&, unsigned char*&, unsigned char&, FreqWord*) const;
public:
	HuffCodeTree(FILE*);
	HuffCodeTree(const unsigned char* const, const unsigned char* const);
	~HuffCodeTree(){ clear(); }
	void encode(FILE*, FILE*) const;
	bool decode(FILE*, FILE*) const;
};	//class HuffCodeTree

#endif	//HUFF_CODE_H

