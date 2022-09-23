/* ICCS227: Project 1: icsh
 * Name: Worawit Penglam
 * StudentID: 6281453
 */

#include "stdio.h"
#include <stdbool.h>
#include <string.h>

#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
	bool isStart = true;
	printf("Starting IC shell\n");
    while (isStart) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
		char *token = strtok(buffer, " ");
		
        printf("you said: %s\n", token);
    }
}
