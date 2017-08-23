#include <stdio.h>
#include <string.h>

#include "huff_code.h"

int main(int argc, char** argv) {

	//parse command
	if(argc!=3){
		fprintf(stderr, "***ERROR int main(int, char**): ");
		fprintf(stderr, "Illegal argumnents.\n");
		fprintf(stderr, "Usage: ./ypf [-c --create | -x --extract] ...\n");
		return 1;
	}

	FILE* input = fopen(argv[2], "rb");
	FILE* output;

	//FileNotExistException handling
	if(input==NULL) {
		fprintf(stderr, "***ERROR int main(int, charr**): ");
		fprintf(stderr, "File does not exist.\n");
		return 1;
	}

	char* pName = new char[strlen(argv[2])+7];
	strcpy(pName, argv[2]);

	//parse option
	if(strcmp(argv[1], "-c")==0 || strcmp(argv[1], "--create")==0) {
		strcat(pName, ".ypf");
		output = fopen(pName, "wb");

		//construct an huffman tree with a file
		//frequency analysis on input
		//should NOT change the position of the file
		HuffCodeTree huff_tree(input);

		//encode the input and pipe out as output
		huff_tree.encode(input, output);
	}
	else if(strcmp(argv[1],"-x")==0 || strcmp(argv[1],"--extract")==0) {
		char* pSubTtl = strrchr(pName, '.');
		if(strcmp(pSubTtl, ".ypf")!=0) {
			//ErrorSubtitleException handling
			fprintf(stderr, "***ERROR int main(int char**): ");
			fprintf(stderr, "Unknown subtitle.\n");
			fprintf(stderr, "\".ypf\" is expected.\n");
			return 1;
		}
		strcpy(pSubTtl, ".unypf");
		output = fopen(pName,"wb");

		//drain information of the huffman coding tree used to encode the input
		//file from the first 320 bytes
		unsigned char in[256];
		unsigned char pre[64];
		int correct = 256;
		correct ^= fread(in, 1, 256, input);
		correct |= 64 ^ fread(pre, 1, 64, input);
		if(correct==0){
			//construct an huffman tree with a given in-order traverse and
			//a pre-order traverse
			HuffCodeTree huff_tree(in,pre);

			//decode the input and pipe out as output
			//note that the input here is not including the information of the tree
			correct = !huff_tree.decode(input,output);
		}
		if(correct!=0) {
			//FileInterruptException handling
			fprintf(stderr,"***ERROR int main(int,char**): ");
			fprintf(stderr,"File ended unexpectedly.\n");
			fprintf(stderr,"There should be a format error; otherwise, the file should be dumpped.\n");
			return 1;
		}

	}
	else {
		//OptionNotExistException handling
		fprintf(stderr,"***ERROR int main(int,char**): ");
		fprintf(stderr,"Illegal options.\n");
		fprintf(stderr,"Usage: ./ypf [-c --create | -x --execute] ...\n");
		return 1;
	}

	fclose(input);
	fclose(output);
	delete[] pName;
	return 0;
}

