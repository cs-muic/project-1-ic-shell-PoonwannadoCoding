#include <stdio.h>
#include <stdlib.h>


#define MAX_LIES 100
#define MAX_LEN 1000
int main(int argc, char* argv[]){

	char ** data[MAX_LIES][MAX_LEN];

	FILE * file;

	file = fopen(argv[1], "r");

	if (file == NULL){

		printf("ERROR");
		return 1;
	}
	int line = 0;

	while(!feof(file) && !ferror(file)){

		if (fgets(data[line], MAX_LEN, file) != NULL){
			line++;
		}
	}

	fclose(file);

	for (int i = 0; i < line; i++){

		printf("%s", data[i]);

	}





	return 0;

}

