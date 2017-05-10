#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "keyvaluestore.h"

//int clientFileDiscriptor;
//char cont[256],file_name[256], c;
int n = 0,rm;

time_t mytime;
int portno;
  char b[256];
pthread_mutex_t gSharedMemoryLock = PTHREAD_MUTEX_INITIALIZER;


void *accept_clients(void *args);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

//kvs_t * head = NULL;

int main(int argc, char *argv[])
{
  struct sockaddr_in sock_var;
  int serverFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
  int clientFileDiscriptor;
  pthread_t t[20];
  int x;
  head = (kvs_t *) malloc(sizeof(kvs_t));

  sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
  sock_var.sin_port=3002;
  sock_var.sin_family=AF_INET;
  if(bind(serverFileDiscriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
     printf("socket has been created\n");
     listen(serverFileDiscriptor,0);

     for(x=0;x<5;x++)      //can support 20 clients at a time
     {
      printf("Main thread: Waiting for new client\n");
      clientFileDiscriptor=accept(serverFileDiscriptor,NULL,NULL);
      printf("Main Thread: Connected to client %d\n",clientFileDiscriptor);
      pthread_create(&t[x],NULL,accept_clients,(void *)clientFileDiscriptor);
     }

      for(x=0;x<3;x++)      //can support 20 clients at a time
      {
        printf("Main thread: doing pthread_join\n");
        pthread_join(t[x], NULL);
      }

    close(serverFileDiscriptor);
   }
   else{
    printf("socket creation failed\n");
   }
   return 0;
  }


  void *accept_clients(void *args){
    int clientFileDiscriptor=(int)args;
    char buffer[256];
    char cont[256],file_name[256];
    int del;

      //    char *aa = "KEY NOT FOUND";
    char *ab = "KEY ALREADY EXIST, BUT NEW VALUE ADDED";
//    printf("Entered the accept client function\n");
//    kvs_t * head = (kvs_t *) malloc(sizeof(kvs_t));

    while(1){
      bzero(buffer,256);
//      printf("Server reached Before read: %s\n",buffer);
      n = read(clientFileDiscriptor,buffer,255);
      printf("Server received after read: %s\n",buffer);
  //    sleep(5);
      if (n <= 0)
      {
      //  break;
         perror("ERROR reading from socket");
         return;
      }

      bzero(file_name,256);
      bzero(cont,256);
      int i,j=0,k,l=0,stoi;
      //const char *errstr;

      k = buffer[0] - '0';

      for(i=2;i<strlen(buffer);i++){
          if(buffer[i] == ';'){
              break;
          }
          file_name[j] = buffer[i];
          j++;
      }

      stoi = atoi(file_name);

      printf("Key: %d\n",stoi);

      i=i+1;
      for(;i<strlen(buffer);i++){
          if(buffer[i] == '\0'){
              break;
          }
          cont[l] = buffer[i];
          l++;
      }

        printf("Value: %s\n",cont);

      kvs_t * temp1;
  //    kvs_t * current;
      int qa;
      char test[256];
      char test1[256];
      switch(k){
          case 1:
            bzero(test1, 256);
            pthread_mutex_lock(&gSharedMemoryLock);
            temp1 = putWrite(stoi, cont);
            sprintf(test1,"Key Value updated, key: %d, VAlue:%s",temp1->key, temp1->name);
            write(clientFileDiscriptor,test1,strlen(test1));
            pthread_mutex_unlock(&gSharedMemoryLock);
            break;

          case 2:
            bzero(test, 256);
            pthread_mutex_lock(&gSharedMemoryLock);
            temp1 = getWrite(stoi);
            if(temp1 == NULL){
              strcpy(test,"KEY NOT FOUND");
            }
            else{
            strcpy(test, temp1->name);
            }
            qa = write(clientFileDiscriptor,test,strlen(test));
            pthread_mutex_unlock(&gSharedMemoryLock);
            if (qa < 0)
                {
                 perror("ERROR writing 324 to socket");
                 return;
                }
            break;

          case 3:
            bzero(test, 256);
            pthread_mutex_lock(&gSharedMemoryLock);
            del = deleteWrite(stoi);
            if(del == 1){
              strcpy(test, "FILE DELETED SUCCESSFULLY");
            }
            else {
              strcpy(test, "FILE NOT DELETED SUCCESSFULLY");
            }
            qa = write(clientFileDiscriptor,test,strlen(test));
            pthread_mutex_unlock(&gSharedMemoryLock);
            if (qa < 0)
                {
                 perror("ERROR writing 324 to socket");
                 return;
                }
              break;
            }
        }
          printf("Before closing new sockfd\n");

          close(clientFileDiscriptor);
          printf("After closing new sockfd\n");
      }
