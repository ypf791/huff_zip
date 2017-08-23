#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "huff_code.h"

using namespace std;

class MinHeap{
private:

	struct MyComp{
		bool operator()(const FreqWord* p1, const FreqWord* p2) const {
			return *p2 < *p1;
		}
	}CMP;	//functional object CMP of struct MyComp

	vector<FreqWord*> data;
public:
	MinHeap(FreqWord* const* _head, FreqWord* const* _tail) {
		data = vector<FreqWord*>(_head, _tail);
		make_heap(data.begin(),data.end(),CMP);
	}
	FreqWord* front() const { return data.front(); }
	void pop(){ 
		pop_heap(data.begin(),data.end(),CMP);
		data.pop_back();
	}
	void push(FreqWord* t) {
		data.push_back(t);
		push_heap(data.begin(),data.end(),CMP);
	}
	bool isEmpty() { return data.size()==0; }
	size_t size() { return data.size(); }
};	//class MinHeap

HuffCodeTree::HuffCodeTree(FILE* input)
	: decoder(0), encoder(0) {
	size_t count[257] = {0};
	double frequency[256] = {0.0};

	//input file analysis
	for(unsigned char buf=fgetc(input); !feof(input); buf=fgetc(input)) {
		++count[buf];
		++count[256];
		}
	rewind(input);

	//leaves construction
	FreqWord** array = new FreqWord*[256];
	for(size_t i=0; i<256; ++i) {
		frequency[i] = ((double)count[i])/((double)count[256]);
		array[i] = new FreqWord(i,frequency[i]);
	}

	//nodes construction
	MinHeap word_heap(array, array+256);
	delete[] array;
	FreqWord *p1,*p2,*tmp;
	do {
		p1 = word_heap.front();
		word_heap.pop();
		p2 = word_heap.front();
		word_heap.pop();
		tmp = new FreqWord('\0', p1->freq+p2->freq, p1, p2);
		word_heap.push(tmp);
	}while(word_heap.size()>1);
	//decoding tree done
	decoder = tmp;

	//encoding array construction
	encoder = new HuffCode[256];
	HuffCode tmp_code = HuffCode();
	givingCode(decoder, tmp_code);
}

HuffCodeTree::HuffCodeTree(const unsigned char* in, const unsigned char* pre) {
	FreqWord** iter = &decoder;
	vector<FreqWord*> r_undef;

	//decoding tree construction
	for(int i=0; i<511; ++i) {
		if(*pre & (unsigned char)(128>>(i&7))) {
			*iter = new FreqWord(*(in++), 0.0);
			iter = &(r_undef.back()->__r);
			r_undef.pop_back();
		}
		else {
			*iter = new FreqWord('\0', 0.0);
			r_undef.push_back(*iter);
			iter = &((*iter)->__l);
		}
		if((i&7)==7) ++pre;
	}

	//encoding array construction
	encoder = new HuffCode[256];
	HuffCode tmp_code = HuffCode();
	givingCode(decoder, tmp_code);
}

void HuffCodeTree::givingCode(const FreqWord* csr, HuffCode& code) {
	if(csr->isLeaf()) encoder[csr->word] = code;
	else {
		if(csr->__l!=0){
			code.append(0);
			givingCode(csr->__l,code);
			code.cuttail();
		}
		if(csr->__r!=0){
			code.append(1);
			givingCode(csr->__r,code);
			code.cuttail();
		}
	}
}

void HuffCodeTree::clear() {
	if(decoder!=0) delete decoder;
	if(encoder!=0) delete[] encoder; 
}

void HuffCodeTree::encode(FILE* input, FILE* output) const {
	//print the encoding tree to the file
	printTree(output);

	//maintain a space for align
	fpos_t align_pos;
	fgetpos(output, &align_pos);
	fputc('\0', output);
	
	//first character encode
	int cut_flg = 0;
	unsigned char _mask = 255;
	unsigned char _buf = fgetc(input);
	unsigned char _align = encoder[_buf].__end;
	const vector<unsigned char>& first_code = encoder[_buf].__code;
	for(size_t i=0; i<first_code.size()-1; ++i) fputc(first_code[i], output);
	_buf = first_code.back();

	//keep encoding
	for(unsigned char ch=fgetc(input); !feof(input); ch=fgetc(input)) {
		//change a name
		const vector<unsigned char>& _code = encoder[ch].__code;
		const unsigned char& _end = encoder[ch].__end;

		//find cut_flg and _mask
		for(cut_flg=0; (1<<cut_flg)!=_align; ++cut_flg);
		_mask = (unsigned char)(255 >> cut_flg);

		for(size_t i=0; i<_code.size()-1; ++i) {
			_buf |= ((~_mask) & _code[i]) >> (8-cut_flg);
			fputc(_buf, output);
			_buf = (_mask & _code[i]) << cut_flg;
		}
		_buf |= ((~_mask) & _code.back()) >> (8-cut_flg);
		if((unsigned char)(_end<<cut_flg)==0) _align = _end >> (8-cut_flg);
		else {
			fputc(_buf, output);
			_buf = (_mask & _code.back()) << cut_flg;
			_align = _end << cut_flg;
		}
	}
	fputc(_buf, output);

	//decide align
	fsetpos(output, &align_pos);
	fputc(_align, output);
}

bool HuffCodeTree::decode(FILE* input, FILE* output) const {
	unsigned char buf, _buf, now_align=1, end_align = fgetc(input);
	buf = _buf = fgetc(input);
	for(FreqWord* iter = decoder; !feof(input); buf=_buf) {
		_buf = fgetc(input);
		now_align = feof(input) ? end_align : 1;
		for(unsigned char flg=1<<7; flg>=now_align; flg>>=1) {
			iter = ((flg&buf)==0) ? iter->__l : iter->__r;
			if(!iter->isLeaf()) continue;
			fputc(iter->word, output);
			iter = decoder;
		}
	}
	return true;
}

void HuffCodeTree::printTree(FILE* output) const {
	if(decoder==0) return;
	unsigned char in_odr[256] = {'\0'};
	unsigned char pre_odr[64] = {'\0'};
	unsigned char* in_flg = in_odr;
	unsigned char* pre_flg = pre_odr;
	unsigned char _mkr = 1<<7;
	parseOrder(in_flg, pre_flg, _mkr, decoder);
	for(size_t i=0; i<256; ++i) fputc(in_odr[i], output);
	for(size_t i=0; i<64; ++i) fputc(pre_odr[i], output);
}

void HuffCodeTree::parseOrder(unsigned char*& in_flg, unsigned char*& pre_flg, unsigned char& _mkr, FreqWord* csr) const {
	if(csr->isLeaf()) {
		*(in_flg++) = csr->word;
		*pre_flg |= _mkr;
	}
	if(_mkr!=1) _mkr >>= 1;
	else {
		_mkr = 1<<7;
		++pre_flg;
	}
	if(csr->__l!=0) parseOrder(in_flg, pre_flg, _mkr, csr->__l);
	if(csr->__r!=0) parseOrder(in_flg, pre_flg, _mkr, csr->__r);
}


