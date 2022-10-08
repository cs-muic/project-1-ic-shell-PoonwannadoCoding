/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <signal.h>
#include <bits/pthreadtypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include<sys/wait.h>
#include<fcntl.h>

// FUNCTION
int command(char **, char *);
char ** splitToken(char *, int);
int exe(char *);
int readLines(char *, char **);
void handle_sigint(int);
int reDir(char *);
void addJobs(pid_t , char*);

// VARIABLE
pid_t pid;
pid_t cpid;
int status;
int sigs;
int isRedirect;
int posFIle;
int isBg;
int bgIndex;
int items;
int id = 1;
struct sigaction new_action, old_action;


#define MAX_CMD_BUFFER 255
#define MAX_LINE_LENGTEH 100
#define MAX_STRING 255
#define MAX_TOKEN 100

typedef struct bg{
	int id;
	char name[MAX_CMD_BUFFER];
	//int status;
	pid_t jobPid;
} job_list;

job_list jobs[100];


int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
	char** rec;
	char history[MAX_CMD_BUFFER];
	int active = 1;
	int i = 0;
	
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
		

		while(waitpid(-1, &sigs, WNOHANG) > 0){}
		
		
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
		
		
		rec = splitToken(buffer, MAX_TOKEN);

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
				char ** prevCommand = splitToken(preC, MAX_TOKEN);
				active = command(prevCommand, history);
				free(preC);
			}
		}

		else if(strcmp(buffer, "\n") != 0){
			active = command(rec, history);
		
		
		}
		pid = 0;
		
	}
	return 0;
}



char ** splitToken(char * args, int limit){
	
	int index = 0;
	char ** tokens = malloc(MAX_TOKEN * sizeof(char *));
	char * token;
	//printf("%s \n", args);
	args[strcspn(args, "\n")] = 0;
	token = strtok(args, " ");
	while(index < MAX_TOKEN && index < limit){
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

				if (strcmp(tokens[index], "&") == 0){
					isBg = 1;
					//printf("OH NOES\n ");
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
	//printf("sig => %d \n", sig);
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
	else if(sig == SIGCHLD && pid){
		//printf("hmm => %s \n", jobs[items].name);
		printf("[%d]+ Done %s \n", jobs[items-1].id, jobs[items-1].name);
		isBg = 0;
		pid = 0;
	}

	
	
}


int command(char ** args, char * buffer){
	

	buffer[strcspn(buffer, "\n")] = 0;
	
	if (args == NULL){
		return 1;
	}

	if (isRedirect > 0){
		char * useForSave;
		useForSave = strdup(buffer);
		reDir(useForSave);
		free(useForSave);
		return 1;
	}

	else if ( strcmp(args[0], "echo") == 0){
		printf("%s", args[1]);
		for(int i = 2; args[i] != NULL; i++){
			printf(" %s", args[i]);
		}
		printf("\n");
		
		return 1;
	}

	else {
		//printf("%s \n", buffer);
		exe(buffer);
			
	}
	return 1;

}

int reDir(char * arg){
	//printf("%s \n", arg);
	char * used;
	used = strdup(arg);
	char ** split;
	split = splitToken(arg, MAX_TOKEN);
	if (isRedirect == 1){
		
		int pfd = open(split[posFIle], O_WRONLY | O_CREAT, 0777);
		int save = dup(1);

		dup2(pfd,1);
		close(pfd);
		exe(used);	
		dup2(save, 1);
		close(save);
		isRedirect = 0;
		

	}
	if (isRedirect == 2){
	
		FILE * fp;
		
		if (fp = fopen(split[posFIle], "r")){
			fclose(fp);
			fp = fopen(split[posFIle], "w");
			fprintf(fp, "%s\n" , used);
			fclose(fp);
			isRedirect = 0;
		} else {
			printf("File not exist \n");
			isRedirect = 0;
			return 1;
		}
			
	}


	free(used);	
	return 1;
}

int exe(char * command){
	char * cpCommand;
	cpCommand = strdup(command);
	
	char ** result;
	
	if (posFIle > 0 || isBg > 0){
		result = splitToken(command, posFIle-1);
		
	}
	else {
		result = splitToken(command, MAX_TOKEN);
		
	}
	
	if ((pid = fork())< 0){
	
		perror("Fork failed");
		
		exit(1);
		
	}
	if(!pid){
		setpgid(0, 0);
		status = execvp(result[0], result);

		if (status < 0 && sigs == 0){
			
			printf("\nBad command \n");
		}
		exit(1);
	}
	if(pid && isBg == 0){
		waitpid(pid, &status, 0);
	}
	if(pid && isBg == 1){

		new_action.sa_handler = SIG_IGN;
		old_action.sa_handler = handle_sigint; 
		sigaction(SIGTTIN, &new_action, NULL);
		sigaction(SIGTTOU, &new_action, NULL);
		sigaction(SIGCHLD, &old_action, NULL);
		
		setpgid(0, 0);//set group id to foreground
		//printf("%d \n", pid);
		//printf("%s \n", cpCommand);
		addJobs(pid, cpCommand);
			
		tcsetpgrp(0, cpid);
		//waitpid(pid, NULL, 0);
		
		tcsetpgrp(0, cpid);
		isBg = 0;
	}
	free(cpCommand);
	return 1;

}

void addJobs(pid_t ppid, char * command){

	jobs[items].id = id;
	strcpy(jobs[items].name, command);
	//jobs[items].name = command;
	//printf("added => %s\n", jobs[items].name);
	jobs[items].jobPid = ppid;
	items++;
	id++;	
	printf("[%d] %d \n", jobs[items-1].id, jobs[items-1].jobPid);

}





