#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

  void kvsPut(int key, char *value);
  void kvsGet(int key2);
  void kvsDelete(int key1);

int sockfd;
time_t mytime;

int main(int argc, char *argv[])
{
  int portno, n, option;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  int flag = 1;
  struct in_addr ip;
  int pton_err;

  if (argc < 3)
  {
      fprintf(stderr,"usage %s port\n", argv[0]);
      exit(1);
  }

  portno = atoi(argv[2]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0){
      error("Error opening socket");
  }

  bzero((char *) &serv_addr,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;

  serv_addr.sin_port = htons(portno);
  pton_err= inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
  if (pton_err <= 0) {
    if (pton_err == 0)
        fprintf(stderr, "Not in presentation format");
    else
        error("inet_pton");
  }

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
  {
      error("ERROR connecting");
  }


    kvsPut(1,"One");
    kvsPut(2,"Two");
    kvsPut(3,"Three");
    kvsPut(4,"Four");
    kvsPut(11,"OneOne");
    kvsPut(22,"TwoTwo");
    kvsPut(33,"ThreeThree");
    kvsPut(44,"FourFour");

    kvsGet(1);
    kvsGet(4);
    kvsGet(22);
    kvsGet(33);
    kvsGet(11);
    kvsGet(4);
    kvsGet(2);
    kvsGet(3);
    kvsGet(44);

    kvsDelete(1);
    kvsDelete(33);
    kvsDelete(4);
    kvsDelete(44);
    kvsDelete(22);
    kvsGet(1);
    kvsGet(33);
    kvsGet(4);
    kvsGet(11);
    kvsGet(2);

    close(sockfd);
    return 0;
  }

    void kvsPut(int key, char *value){
        int c = 1,l, p;
        char buffer1[256];
        bzero(buffer1, 256);
        printf("Key value \n");
        sprintf(buffer1,"%d:%d;%s", c,key, value);
        mytime = time(NULL);
        printf("PUT service is requested at: %s",ctime(&mytime));
        l = write(sockfd,buffer1,strlen(buffer1));
        if (l < 0)
        {
            error("ERROR writing to socket");
        }
          bzero(buffer1, 256);
          p = read(sockfd,buffer1,255);
          mytime = time(NULL);
          printf("PUT service response is received at: %s",ctime(&mytime));
          if (p < 0)
          {
              error("ERROR reading from socket");
          }
          printf("Put: %s\n",buffer1);
    }

    void kvsGet(int key2){
      int c = 2, p, l;
      char buffer2[256];
      char buffer4[256];
      bzero(buffer2, 256);
      sprintf(buffer2,"%d:%d",c,key2);
      mytime = time(NULL);
      printf("GET service is requested at: %s",ctime(&mytime));
      l = write(sockfd,buffer2,strlen(buffer2));
      if (l < 0)
      {
          error("ERROR writing to socket");
      }
      bzero(buffer4,256);
      p = read(sockfd,buffer4,255);
      mytime = time(NULL);
      printf("GET service response is received at: %s",ctime(&mytime));
      if (p < 0)
      {
          error("ERROR reading from socket");
      }
      printf("Value of %d: %s\n", key2, buffer4);
    }

    void kvsDelete(int key1){
      int c = 3, l, p;
      char buffer3[256];
      char buffer5[256];
      bzero(buffer3,256);
      sprintf(buffer3,"%d:%d",c,key1);
      mytime = time(NULL);
      printf("DELETE service is requested at: %s",ctime(&mytime));
      l = write(sockfd,buffer3,strlen(buffer3));
      printf("Key: %d\n", key1);
      if (l < 0)
      {
          error("ERROR writing to socket");
      }
      bzero(buffer5,256);
      p = read(sockfd,buffer5,255);
      mytime = time(NULL);
      printf("DELETE service response is received at: %s",ctime(&mytime));
      if (p < 0)
      {
          error("ERROR reading from socket");
      }
      printf("%s\n",buffer5);
    }
