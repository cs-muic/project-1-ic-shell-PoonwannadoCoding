/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include<sys/wait.h>
#include<fcntl.h>


// FUNCTION
int command(char **, char *, int);
char ** splitToken(char *);
int exe(char *, int);
int readLines(char *, char **);
void handle_sigint(int);


// VARIABLE
pid_t pid;
int status;
int sigs;
int isRedirect;
int posFIle;

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

	struct sigaction new_action, old_action;


	
	printf("Starting IC shell\n");
    while (active) {


		new_action.sa_handler = handle_sigint;
		sigemptyset(&new_action.sa_mask);
		new_action.sa_flags = 0;

		sigaction(SIGINT, NULL , &old_action);
		if (old_action.sa_handler != SIG_IGN){
			sigaction(SIGINT, &new_action, NULL);
		}

		sigaction(SIGTSTP, NULL, &old_action);
		if (old_action.sa_handler != SIG_IGN){
			sigaction(SIGTSTP, &new_action, NULL);
		}

		while(waitpid(pid, &sigs, WNOHANG) > 0);
		
		
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

		else{
			fgets(buffer, 255, stdin);
			
		}
	

		if (strcmp(buffer, "!!\n") != 0 && strcmp(buffer, "\n") != 0){
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
			if (strcmp(history, "") == 0){

				printf("Bad command \n");
			} else {
				char * preC= strdup(history);
				char ** prevCommand = splitToken(preC);
				active = command(prevCommand, history, argc);
				free(preC);
			}
		}

		else if(strcmp(buffer, "\n") != 0){
			active = command(rec, history, argc);
		
		}
		pid = 0;
		
	}
	return 0;
}



char ** splitToken(char * args){
	
	int index = 0;
	char ** tokens = malloc(MAX_TOKEN * sizeof(char *));
	char * token;
	//printf("%s \n", args);
	token = strtok(args, " ");
	while(index < MAX_TOKEN){
		if(token != NULL){

			tokens[index] = token;

			if (tokens[index] != NULL){
				
				if(strcmp(tokens[index], ">") == 0){
					isRedirect = 1;
					posFIle = index + 1;
				}

				if (strcmp(tokens[index], "<") == 0){
					isRedirect = 2;
					posFIle = index + 1;
				}
			}
			
		
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

void handle_sigint(int sig){
	sigs = sig;
	if (sig == SIGTSTP && pid){
		kill(pid, SIGTSTP);
		printf("\n");
		return; 
		
	}
	
	else if (sig == SIGINT && pid){
		kill(pid, SIGINT);
		waitpid(pid, &sigs, 0);
		printf("\n");
	}

	
	
}


int command(char ** args, char * buffer, int argc){
	
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
		exe(buffer, argc);
			
	}
	return 1;

}


int exe(char * command, int argc){
	
	
	char ** result;
	
	
	
	command[strcspn(command, "\n")] = 0;

	result = splitToken(command);
	
	
	//printf("is Redirect = %d \n", isRedirect);
	if (isRedirect > 0){
		if (isRedirect == 2){
			//printf("WW %s \n", result[posFIle]);
			FILE * fp;
			fp = fopen(result[posFIle], "w");
			fprintf(fp, "%s\n", command);
			fclose(fp);
			
			isRedirect = 0;
			return 1;
		}
		
	}

	
	if ((pid = fork())< 0){
	
		perror("Fork failed");
		
		exit(1);
		
	}
	if(!pid){
		status = execvp(result[0], result);
		if (status < 0 && sigs == 0){
			
			printf("\nBad command \n");
		}
		exit(1);
	}
	if(pid){
		waitpid(pid, &status, 0);
		
	}
	return 1;

}





