#define _GNU_SOURCE
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

int n = 0,rm;
time_t mytime;
int portno;
char b[256];
time_t mytime;
pthread_mutex_t gSharedMemoryLock = PTHREAD_MUTEX_INITIALIZER;

void *accept_clients(void *args);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
  int portno, clilength;
  int serverFileDiscriptor;
  struct sockaddr_in serv_addr, cli_addr;
  int clientFileDiscriptor;
  pthread_t t[20];
  int x;
  int yes=1;
  int cur_cpu = 0;
  cpu_set_t cpuset;
  int s, j;
  head = (kvs_t *) malloc(sizeof(kvs_t));

  if (argc < 2)
      {
          fprintf(stderr,"ERROR, no port provided\n");
          exit(1);
      }

  serverFileDiscriptor = socket(AF_INET, SOCK_STREAM, 0);

  if (serverFileDiscriptor < 0)
      {
          error("ERROR opening socket");
      }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (setsockopt(serverFileDiscriptor,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
      perror("setsockopt");
      exit(1);
  }

  if (bind(serverFileDiscriptor, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) >= 0)
  {
    listen(serverFileDiscriptor,5);
    clilength = sizeof(cli_addr);

   long total_cores = sysconf(_SC_NPROCESSORS_ONLN);

     for(x=0;x<5;x++)
     {
      CPU_ZERO(&cpuset);
      printf("Main thread: Waiting for new client\n");
      clientFileDiscriptor=accept(serverFileDiscriptor,(struct sockaddr *) &cli_addr, &clilength);
      printf("Main Thread: Connected to client %d\n",clientFileDiscriptor);
      pthread_create(&t[x],NULL,accept_clients,(void *)&clientFileDiscriptor);
      CPU_SET(cur_cpu, &cpuset);
      s = pthread_setaffinity_np(t[x], sizeof(cpu_set_t), &cpuset);
      if(s != 0){
        printf("Set Affinity failed\n");
        continue;
      }
      s = pthread_getaffinity_np(t[x], sizeof(cpu_set_t), &cpuset);
      if (s != 0){
        printf("Get Affinity failed\n");
        continue;
      }
      printf("Set returned by pthread_getaffinity_np() contained:\n");
               printf("    CPU %d\n", cur_cpu);
      cur_cpu = (cur_cpu + 1) % total_cores;
     }

      for(x=0;x<5;x++)
      {
        printf("Main thread: doing pthread_join\n");
        pthread_join(t[x], NULL);
      }
    close(serverFileDiscriptor);
   }
   else
   {
    printf("socket creation failed\n");
   }
   return 0;
  }

  void *accept_clients(void *args){
    int *arg = (int*)args;
    int clientFileDiscriptor=*arg;
    char buffer[256];
    char cont[256],key_name[256];
    int del;
    char *ab = "KEY ALREADY EXIST, BUT NEW VALUE ADDED";

    while(1){
      bzero(buffer,256);
      n = read(clientFileDiscriptor,buffer,255);
      printf("Server received after read: %s\n",buffer);
      if (n <= 0)
      {
         perror("ERROR reading from socket");
         return NULL;
      }

      bzero(key_name,256);
      bzero(cont,256);
      int i,j=0,k,l=0,stoi;

      k = buffer[0] - '0';

      for(i=2;i<strlen(buffer);i++){
          if(buffer[i] == ';'){
              break;
          }
          key_name[j] = buffer[i];
          j++;
      }

      stoi = atoi(key_name);

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
      int qa;
      char test[256];
      char test1[256];
      switch(k){
          case 1:
            bzero(test1, 256);
            pthread_mutex_lock(&gSharedMemoryLock);
            mytime = time(NULL);
            printf("PUT service request is received at: %s",ctime(&mytime));
            temp1 = putWrite(stoi, cont);
            sprintf(test1,"Key Value updated, key: %d, VAlue:%s",temp1->key, temp1->name);
            write(clientFileDiscriptor,test1,strlen(test1));
            mytime = time(NULL);
            printf("PUT service response is sent at: %s",ctime(&mytime));
            pthread_mutex_unlock(&gSharedMemoryLock);
            break;

          case 2:
            bzero(test, 256);
            pthread_mutex_lock(&gSharedMemoryLock);
            mytime = time(NULL);
            printf("GET service is requested at: %s",ctime(&mytime));
            temp1 = getWrite(stoi);
            if(temp1 == NULL){
              strcpy(test,"KEY NOT FOUND");
            }
            else{
            strcpy(test, temp1->name);
            }
            qa = write(clientFileDiscriptor,test,strlen(test));
            mytime = time(NULL);
            printf("GET service response is sent at: %s",ctime(&mytime));
            pthread_mutex_unlock(&gSharedMemoryLock);
            if (qa < 0)
                {
                 perror("ERROR writing 324 to socket");
                 return NULL;
                }
            break;

          case 3:
            bzero(test, 256);
            pthread_mutex_lock(&gSharedMemoryLock);
            mytime = time(NULL);
            printf("DELETE service is requested at: %s",ctime(&mytime));
            del = deleteWrite(stoi);
            if(del == 1){
              strcpy(test, "FILE DELETED SUCCESSFULLY");
            }
            else {
              strcpy(test, "FILE NOT DELETED SUCCESSFULLY");
            }
            qa = write(clientFileDiscriptor,test,strlen(test));
            mytime = time(NULL);
            printf("DELETE service response is sent at: %s",ctime(&mytime));
            pthread_mutex_unlock(&gSharedMemoryLock);
            if (qa < 0)
                {
                 perror("ERROR writing 324 to socket");
                 return NULL;
                }
              break;
            }
        }
      printf("Before closing new FileDiscriptor\n");

      close(clientFileDiscriptor);
      printf("After closing new FileDiscriptor\n");
  }
