/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <signal.h>
#include <bits/pthreadtypes.h>
#include <bits/types/sigset_t.h>
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
void deleteJobs(int);
void handle_child(int);
void jobDone(pid_t, int);
void clearJob(void);
void printJob(void);
void fg(int);

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
int recieve;
struct sigaction new_action, old_action, fg_action, bg_action;


#define MAX_CMD_BUFFER 255
#define MAX_LINE_LENGTEH 100
#define MAX_STRING 255
#define MAX_TOKEN 100

typedef struct bg{
	int id;
	char name[MAX_CMD_BUFFER];
	int status;
	pid_t jobPid;
} job_list;

job_list jobs[100];


int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
	char** rec;
	char history[MAX_CMD_BUFFER];
	int active = 1;
	int i = 0;
	cpid = getpid();
	new_action.sa_handler = handle_sigint;
	old_action.sa_handler = SIG_IGN;
	fg_action.sa_handler = handle_child; 

	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGCHLD, &fg_action, NULL);
	sigaction(SIGINT, &new_action, NULL);
	sigaction(SIGTSTP, &new_action, NULL);
	sigaction(SIGTTIN, &old_action, NULL);
	sigaction(SIGTTOU, &old_action, NULL);

	//printf("cpid %d \n", cpid);	
	printf("Starting IC shell\n");
    while (active) {

		if (isBg == 1){
			isBg = 0;
			continue;
		}
		
		
		clearJob();

		while (waitpid(-1, &sigs, WNOHANG) > 0) {}
		
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

		if(strcmp(buffer, "\n") == 0) continue;
	

		if (strcmp(buffer, "!!\n") != 0 && strcmp(buffer, "\n") != 0){
			strcpy(history, buffer);
			
		}

		if (recieve){
			memset(buffer, 0, 255);
			recieve = 0;
			printf("\n");
			continue;

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
			//printf("SUS 0 \n");
			active = command(rec, history);
			
		}
		//clearJob();
		pid = 0;
	
	
		
		
		
	}
	
	fflush(stdin);
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

void printJob(){
	for(int i = 0; i < items; i++){
		if (items - i == 2){
			if (jobs[i].status == 1){
				printf("[%d]- Running \t \t \t \t %s \n", jobs[i].id, jobs[i].name);
			}
			else{
				printf("[%d]- Stopped \t \t \t \t %s \n", jobs[i].id, jobs[i].name);
			}
		
		}
		else if(items -i == 1){
			if (jobs[i].status == 1){
				printf("[%d]+ Running \t \t \t \t %s \n", jobs[i].id, jobs[i].name);
			
			}
			else{
				printf("[%d]+ Stopped \t \t \t \t %s \n", jobs[i].id, jobs[i].name);
				
			}

		} else {
			if (jobs[i].status == 1){
				printf("[%d] Running \t \t \t \t %s \n", jobs[i].id, jobs[i].name);
			}
			else {
				printf("[%d] Stopped \t \t \t \t %s \n", jobs[i].id, jobs[i].name);
			}
		}
	}


}

void handle_sigint(int sig){
	sigs = sig;

	//while (waitpid(-1, &sigs, 0)) {}

	//printf("sig => %d \n", sig);
	if (sig == SIGTSTP && pid){
		kill(pid, SIGTSTP);
		printf("\n");
		return; 
		
	}
	
	else if (sig == SIGINT && pid){
		kill(pid, SIGINT);
		//waitpid(-1, &sigs, 0);
		printf("\n");
		return;
	}
	printf("\n");

	
	
}


int command(char ** args, char * buffer){
	
	//printf("buffer => %s\n", buffer);	
	buffer[strcspn(buffer, "\n")] = 0;
	buffer[strcspn(buffer, "%")] = 0;
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
	else if (strcmp(args[0], "jobs") == 0){
		printJob();
		return 1;
	
	}

	else if (strcmp(args[0], "fg") == 0){
		
		fg(atoi(args[1]));
		
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
		
		if (fopen(split[posFIle], "r")){
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
	
	sigset_t new_set;
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &new_set, NULL);
	pid = fork();
	
	char ** result;
	
	if (posFIle > 0 || isBg > 0){
		result = splitToken(command, posFIle-1);	
	}
	else {
		result = splitToken(command, MAX_TOKEN);
	}
	
	if (pid< 0){
	
		perror("Fork failed");	
		exit(1);
		
	}
	if(pid == 0){

		sigprocmask(SIG_UNBLOCK, &new_set, NULL);
		setpgid(0, 0);
		tcsetpgrp(0, getpid());
		if (isBg == 1){
			tcsetpgrp(0, cpid);
		}
		status = execvp(result[0], result);
		if (status < 0 && sigs == 0){
			printf("\nBad command \n");
		}
		exit(1);
	}
	
	else {
		setpgid(pid, pid);
		tcsetpgrp(0, pid);
		
		if (isBg == 1){
			addJobs(pid, cpCommand);
		}
		
		sigprocmask(SIG_UNBLOCK, &new_set, NULL);

		if (isBg == 0){
			waitpid(pid, &status, WUNTRACED);
			if (WIFSIGNALED(status)){
				printf("\n");
			}

			if (WIFSTOPPED(status)){

				printf("MEEP \n");
			}
		}
		tcsetpgrp(0, cpid);
	}
	//sigprocmask(SIG_SETMASK, &old_set, NULL);
	free(cpCommand);
	return 1;

}

void addJobs(pid_t ppid, char * cmp){

	jobs[items].id = id;
	strcpy(jobs[items].name, cmp);
	//jobs[items].name = command;
	//printf("added => %s\n", jobs[items].name);
	jobs[items].jobPid = ppid;
	jobs[items].status = 1;
	items++;
	id++;	
	printf("[%d] %d \n", jobs[items-1].id, jobs[items-1].jobPid);

}

void jobDone(pid_t ppid, int status){
	for (int i = 0; i < 100; i++){
		if (jobs[i].jobPid == ppid) {
			jobs[i].status = status;
			break;
		}
	}

}


void deleteJobs(int id){
	
	for (int i = id-1; i < 100; i++) {
		items --;
		jobs[i].id = jobs[i+1].id-1;
		strcpy(jobs[i].name, jobs[i+1].name);
		jobs[i].jobPid = jobs[i+1].jobPid;
		jobs[i].status = jobs[i+1].status;
	}

	return;

}



void handle_child(int sigs){

	int ppid = 0;
	int status = 0;
	while ((ppid = waitpid(-1, &status, WNOHANG)) > 0){
		recieve = 1;
		jobDone(ppid, 2);
	}

	return;

}

void clearJob(){
	for(int i = 0; i < 100; i++){

		if (jobs[i].status == 2){
			char * commandName = strdup(jobs[i].name);
			commandName[strcspn(commandName, "&")]=0;
			printf("[%d]+ Done \t \t \t \t %s \n", jobs[i].id, commandName);
			deleteJobs(jobs[i].id);
		}
		
	}
}

void fg(int id){
	int status = 0;
	for(int i = 0; i < 100; i++){
		if (jobs[i].id == id){
			kill(SIGCONT, jobs[i].jobPid);
			tcsetpgrp(0, jobs[i].jobPid);
			waitpid(-1, &status, 0);
			tcsetpgrp(0, cpid);
			break;
		}

	}


}



