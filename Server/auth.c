#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "database.h"

char user[350];
int verified = 0;

// Check account with MySQL database
char *checkAccount(char *userAccount, char *password) {
    if (verified) return "213";  // Already logged in
    
    int user_id;
    int result = db_user_login(userAccount, password, &user_id);
    
    if (result == 200) {
        // Login successful
        verified = 1;
        strcpy(user, userAccount);
        return "200";
    } else if (result == 211) {
        // User not found or wrong password
        return "211";
    } else {
        // Database error
        return "000";
    }
}

// Create account with MySQL database
char *makeAccount(char *userAccount, char *password) {
    if (verified) return "213";  // Already logged in
    
    int result = db_user_register(userAccount, password);
    
    if (result == 100) {
        // Registration successful
        verified = 1;
        strcpy(user, userAccount);
        return "100";
    } else if (result == 111) {
        // Username already exists
        return "111";
    } else {
        // Database error
        return "000";
    }
}

// Process auth command (legacy function - kept for compatibility)
char *takeAuthCommand(char* cmd) {
    char *command = strtok(cmd, " ");
    char *account = strtok(NULL, " ");
    char *password = strtok(NULL, " ");
    
    if (strlen(account) > 350 || strlen(password) > 30) return "112";
    
    char cmdCode[4];
    
    if (strcmp(command, "LOGIN") == 0) {
        strcpy(cmdCode, checkAccount(account, password));
    } else if (strcmp(command, "REGISTER") == 0) {
        strcpy(cmdCode, makeAccount(account, password));
    }
    cmdCode[3] = '\0';
    
    if (strcmp(cmdCode, "000") == 0) {
        printf("Database error occurred\n");
    } else if (strcmp(cmdCode, "100") == 0 || strcmp(cmdCode, "200") == 0) {
        verified = 1;
        strcpy(user, account);
    }
    
    return cmdCode;
}
