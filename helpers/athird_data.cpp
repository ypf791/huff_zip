#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../huff_code.h"

int main(){
	srand(time(NULL));
	FILE* fout = fopen("test.t1", "wb");
	bool cur = false;
	for(int i=0;i<10000;i++){
		HuffCode code;
		for(int j=0;j<8;j++){
			cur = (rand()%3 == 0) ? true : false;
			code.append(cur ? 1 : 0);
		}
		code.printCode(fout);
	}
}
