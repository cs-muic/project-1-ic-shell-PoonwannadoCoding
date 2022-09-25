/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int command(char **);
char ** splitToken(char *);
char ** readFile(char *);

#define MAX_LINE_LENGTEH 100
#define MAX_CMD_BUFFER 255
#define MAX_STRING 255
#define MAX_TOKEN 100

int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
	char** rec;
	char history[MAX_CMD_BUFFER];
	int active = 1;
	int i = 0;
	
	

	printf("Starting IC shell\n");
    while (active) {
		

        printf("icsh $ ");

		if (argc > 1){
			
			char total[MAX_CMD_BUFFER][MAX_LINE_LENGTEH];
			FILE * textFile;
			int index = 0;
			textFile = fopen(argv[1], "r");
			if (textFile == NULL){
				return 1;
			}
			while (!feof(textFile) && !ferror(textFile)) {
				if(fgets(total[index], MAX_LINE_LENGTEH, textFile) != NULL){
					index++;
		
				}
			}
			fclose(textFile);
			strcpy(buffer, total[i]);
			i++;

		}

		else {
			fgets(buffer, 255, stdin);
		}
		printf("buffer => %s", buffer);

		if (strcmp(buffer, "!!\n") != 0){
			strcpy(history, buffer);
			
		}
		

		rec = splitToken(buffer);

		if(strcmp(rec[0], "exit") == 0){
			printf("Bye \n");
			int arg1 = atoi(rec[1]);
			if (arg1 > 255){
				return arg1 >> 8;
			} else{
			return arg1;
			}
		}

		else if(strcmp(rec[0], "!!\n") == 0 && strcmp(history, "!!\n") != 0){

			char ** prevCommand = splitToken(history);
			active = command(prevCommand);
		}


		else{
			
			active = command(rec);
		}		
	}
	return 0;
}

char ** readFile(char * filename){
	char ** total[MAX_CMD_BUFFER][MAX_LINE_LENGTEH];
	FILE * textFile;
	int index = 0;
	textFile = fopen(filename, "r");
	if (textFile == NULL){
		return NULL;
	}
	while (!feof(textFile) && !ferror(textFile)) {
		if(fgets(total[index], MAX_LINE_LENGTEH, textFile) != NULL){
			index++;
		
		}
	}
	fclose(textFile);
	
	printf("read this shit => %s \n", total[0]);
	
	return total;
}

char ** splitToken(char * args){
	
	int index = 0;
	char ** tokens = malloc(MAX_TOKEN * sizeof(char *));
	char * token;

	token = strtok(args, " ");
	while(token != NULL){
		tokens[index] = token;
		index ++;

		if(index >= (MAX_TOKEN - 1)){
			break;
		}
		token = strtok(NULL, " ");
	}
	
	return tokens;
}


int command(char ** args){


	if (args == NULL){
		return 1;
	}
	else if ( strcmp(args[0], "echo") == 0){
		printf("%s", args[1]);
		for(int i = 2; args[i] != NULL; i++){
			printf(" %s", args[i]);
		}
		
		return 1;
	}
	else {
		printf("Bad command\n");
	}
	return 1;

}



