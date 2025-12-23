#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ultility.h"

int parse_message(char *message, char params[10][100]) {
	int paramCount = 0;
	char *token = strtok(message, " ");
	while (token != NULL && paramCount < 10) {
		strcpy(params[paramCount], token);
		paramCount++;
		token = strtok(NULL, " ");
	}
	return paramCount;
}