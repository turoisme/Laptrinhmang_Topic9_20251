#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#define BUFF_SIZE 8193
//global variable****************************************************
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
struct sockaddr_in check;
int client_sock;                               //client socket
char send_data[BUFF_SIZE];                     //data to send
char recv_data[BUFF_SIZE];                     //receiving data
int byte_send,byte_recv;                       //number of byte sent\received
socklen_t sin_size;                                  //sockaddr size
//error message******************************************************
void errorMessage(int errCode){
  switch(errCode){
  case 1:printf("Client: The program takes exactly 2 arguements\n");break;
  case 2:printf("Client: Invalid input(s)\n");break;
  case 3:printf("Client: Problem at creating client socket\n");break;
  case 4:printf("Client: Problem at connecting server socket\n");break;
  case 5:printf("Client: Connection terminated by server\n");break;
  }
}


//make new client****************************************************
void makeClient(char *argv[]){
  sin_size=sizeof(struct sockaddr_in);
  client_sock=socket(AF_INET,SOCK_STREAM,0); //Create client socket
  if(client_sock<0){
    errorMessage(3);                         //Create socket failed 
  }
  memset(&(server_addr.sin_zero),0,sizeof(server_addr.sin_zero)); //set up socket
  server_addr.sin_family=AF_INET;            //IPv4
  server_addr.sin_port=htons(atoi(argv[2])); //Connect to port
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr); //Write server IP to 32 bits
  if(connect(client_sock,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1){
    errorMessage(4);                         //Fail to connect
  };
  runClient();                               //Start client
}
int main(int argc,char *argv[]){
  if(argc!=3)errorMessage(1);
  else if(inet_pton(AF_INET, argv[1], &client_addr.sin_addr)==0||atoi(argv[2])<=0){
    errorMessage(2);
  }
  else{
    makeClient(argv);
  }
}
