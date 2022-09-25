/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>

int command(char **, char *);
char ** splitToken(char *);
int exe(char *);

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

		else if((strcmp(rec[0], "!!\n") == 0 && strcmp(history, "!!\n") != 0)|| strcmp(rec[0], "!!") == 0){

			char ** prevCommand = splitToken(history);
			active = command(prevCommand, history);
		}


		else{
			
			active = command(rec, history);
		}		
	}
	return 0;
}


char ** splitToken(char * args){
	
	int index = 0;
	char ** tokens = malloc(MAX_TOKEN * sizeof(char *));
	char * token;

	token = strtok(args, " ");
	while(index < MAX_TOKEN){
		if(token != NULL){
			tokens[index] = token;
		} else{
			tokens[index] = NULL;
		}
		index ++;

		if(index >= (MAX_TOKEN - 1)){
			break;
		}
		token = strtok(NULL, " ");
	
	}
	
	
	return tokens;
}


int command(char ** args, char * buffer){
	
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
		exe(buffer);
		
			
			
			
	}
	return 1;

}


int exe(char * command){
	
	int pid;
	char ** result;
	int status;
	
	command[strcspn(command, "\n")] = 0;

	result = splitToken(command);
	
	if ((pid = fork())< 0){
	
		perror("Fork failed");
		exit(1);
		
	}
	if(!pid){
		status = execvp(result[0], result);
		if (status == -1){
			printf("Bad command \n");
		}
		exit(1);
	}
	if(pid){
		waitpid(pid, NULL, 0);
		
	}
	return 1;

}



