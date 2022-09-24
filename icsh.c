/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int command(char **);
char ** splitToken(char *);


#define MAX_CMD_BUFFER 255
#define MAX_STRING 255
#define MAX_TOKEN 100

int main() {
    char buffer[MAX_CMD_BUFFER];
	char** rec;
	char history[MAX_CMD_BUFFER];
	int active = 1;
	int index = 0;

	printf("Starting IC shell\n");
    while (active) {

        printf("icsh $ ");
        fgets(buffer, 255, stdin);



		if (strcmp(buffer, "!!\n") != 0){
			strcpy(history, buffer);
			
		}
		

		rec = splitToken(buffer);

		if(strcmp(rec[0], "exit") == 0){
			printf("Bye \n");
			return atoi(rec[1]);
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
	tokens[index] == NULL;
	return tokens;
}


int command(char ** args){


	if (args[0] == NULL){
		return 1;
	}
	else if ( strcmp(args[0], "echo") == 0){
		for(int i = 1; args[i] != NULL; i++){
			printf("%s ", args[i]);
		}
		printf("\n");
		return 1;
	}
	else {
		printf("Un known command\n");
	}
	return 1;

}



