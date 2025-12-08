// auth.h - Xử lý đăng ký và đăng nhập
// Quản lý tài khoản người dùng, phiên đăng nhập

#ifndef AUTH_H
#define AUTH_H

char *takeAuthCommand(char* cmd);
extern char user[350];
extern int verified;

#endif
