#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	FILE* ifile = fopen(argv[1], "rb");
	char name[] = "t1.log";
	FILE* ofile = fopen(name, "w");
	int i=0;
	for(unsigned char ch=fgetc(ifile); !feof(ifile); ch=fgetc(ifile)) {
		fprintf(ofile, "%-3d ", ch);
		if(((++i)&15)==0) fprintf(ofile, "\n");
	}
	fprintf(ofile, "\n");
}
