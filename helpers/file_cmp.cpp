#include <stdio.h>

int main(int argc, char** argv){
	FILE* f1 = fopen(argv[1],"rb");
	FILE* f2 = fopen(argv[2],"rb");
	if(f1==0 || f2==0) {
		fprintf(stderr, "***ERROR file_cmp: ");
		fprintf(stderr, "File(s) not found\n");
		return 1;
	}
	char a, b;
	while(!(feof(f1)) && !(feof(f2))){
		a = fgetc(f1);
		b = fgetc(f2);
		if(a!=b){
			printf( "[beep]\n");
			return 0;
		}
	}
	printf("congratulation!!\n");
	return 0;
}
