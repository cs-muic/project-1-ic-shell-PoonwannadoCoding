/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <stdlib.h>
#include <string.h>


#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
	
	printf("Starting IC shell\n");
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
		
		char *token = strtok(buffer, " "); // split the input
		
		if (strcmp(token, "echo") == 0){
			token = strtok(NULL, " ");	
			while(token != NULL){
				printf("%s ", token);
				token = strtok(NULL, " ");
			}
			
		}

		else if (strcmp(token, "exit") == 0){
		int result = atoi(strtok(NULL, " "));
			
			return (int) result;
		}
		

        
    }
}



