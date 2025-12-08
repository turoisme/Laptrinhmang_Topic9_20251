<<<<<<< HEAD
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"

//Temporary save user info and current verification
char user[350];
int verified=0;

int authUser(char *searchPassword,char *password){
	//Encrypting argorithm can be done here (if possible)
	if(strcmp(password,searchPassword)==0)return 1;
	else return 0;
}
char *checkAccount(char *userAccount, char *password){
  if(verified)return "213";
  FILE *accountList=fopen("account.txt","r");
  if(accountList==NULL){
    return "000";
  } //Open file, return error if file not found
  char searchAccount[350];
  char searchPassword[30];
  int status=0;
  while(fscanf(accountList,"%s %s %d",searchAccount,searchPassword,&status)!=EOF){
    if(strcmp(userAccount,searchAccount)==0){
    	if(authUser(searchPassword,password)){
    		fclose(accountList);
    		return "200";
		}
	}
  }
  fclose(accountList);
  return "211";
}

char *makeAccount(char *userAccount, char *password){
  if(verified)return "213";
  FILE *accountList=fopen("account.txt","a+");
  if(accountList==NULL){
    return "000";
  } //Open file, return error if file not found
  char searchAccount[350];
  char searchPassword[30];
  int status=0;
  while(fscanf(accountList,"%s %s %d",searchAccount,searchPassword,&status)!=EOF){
    if(strcmp(userAccount,searchAccount)==0){
    	fclose(accountList);
    	return "111";
	}
  }
  fprintf(accountList,"%s %s %d\n",userAccount,password,1);
  fclose(accountList);
  return "200";
}

char *takeAuthCommand(char* cmd){
  char *command=strtok(cmd," ");
  char *account=strtok(NULL," ");
  char *password=strtok(NULL," ");
  if(strlen(account)>350||strlen(password)>30)return "112";
  char cmdCode[4];
  
  if(strcmp(command,"LOGIN")==0)strcpy(cmdCode,checkAccount(account,password));
  else if(strcmp(command,"REGISTER")==0)strcpy(cmdCode,makeAccount(account,password));
  cmdCode[3]='\0';
  
  if(strcmp(cmdCode,"000")==0)printf("User account file not found, require account.txt");
  else if(strcmp(cmdCode,"100")==0||strcmp(cmdCode,"200")==0){
  	verified=1;
	  strcpy(user,account);
  }
  return cmdCode;
}
=======
// auth.c - Authentication implementation

#include "auth.h"

// TODO: Implement authentication functions
>>>>>>> e2704817790d36ea43239e6d3334d2dabba29b2e
