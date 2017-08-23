#include <stdio.h>

#include "huff_code.h"

void HuffCode::append(bool bit){
	if(__end==1){
		__end = 1<<7;
		__code.push_back(bit ? __end : 0);
	}
	else{
		__end >>= 1;
		if(bit) __code.back() |= __end;
		else __code.back() &= (~__end);
	}
}

void HuffCode::cuttail(){
	if(__code.size()==0)return;
	if(__end==1<<7){
		__end = 1;
		__code.pop_back();
	}
	else{
		__code.back() &= (~__end);
		__end <<= 1;
	}
}

