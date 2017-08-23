#include <stdio.h>
#include <string.h>

#include "huff_code.h"
int main(int argc, char** argv){
	char str_o1[80];
	char str_o2[80];
	strcpy(str_o1, argv[1]);
	strcpy(str_o2, argv[1]);
	strcat(str_o1, ".ypf");
	strcat(str_o2, ".unypf");
	FILE* file_in = fopen(argv[1], "rb");
	FILE* file_ypf = fopen(str_o1, "wb");
	FILE* file_unypf = fopen(str_o2, "wb");
	HuffCodeTree huff_tree(file_in);
	huff_tree.encode(file_in, file_ypf);
	fclose(file_ypf);
	file_ypf = fopen(str_o1, "rb");
	for(int i=0;i<320;++i)fgetc(file_ypf);
	huff_tree.decode(file_ypf, file_unypf);
	return 0;
}

