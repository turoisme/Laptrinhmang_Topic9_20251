#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "logger.h"

int print_log(char *message,int sockfd,int return_code){
    FILE *logger=fopen("server.log","a");
    if( logger==NULL)return 0;
    char log[1000];
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    char time_str[100];
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    if(return_code)sprintf(log, "[%s] [sockfd: %d] [return_code: %d] %s\n", time_str, sockfd, return_code, message);
    else sprintf(log, "[%s] [sockfd: %d] [return_code: 000] %s\n", time_str, sockfd, message);
    fprintf(logger, "%s", log);
    fclose(logger);
    return 1;
}
