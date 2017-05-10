#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

  void kvsPut(int key, char *value);
  void kvsGet(int key2);
  void kvsDelete(int key1);


//int sockfd;
int clientFileDiscriptor;

int main(int argc, char *argv[])
{
  /*
    int portno, n, option;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int flag = 1;

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

    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr,"Error! No such host exist\n");
        exit(1);
    }

    bzero((char *) &serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    */
    struct sockaddr_in sock_var;
    clientFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
    char str_clnt[20],str_ser[20];

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=3002;
    sock_var.sin_family=AF_INET;

    if(connect(clientFileDiscriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {

    kvsPut(1,"One");
    sleep(1);
    kvsPut(2,"Two");
    sleep(1);
    kvsPut(3,"Three");
    sleep(1);
    kvsPut(4,"Four");
    sleep(1);
    kvsPut(11,"OneOne");
    sleep(1);
    kvsPut(22,"TwoTwo");
    sleep(1);
    kvsPut(33,"ThreeThree");
    sleep(1);
    kvsPut(44,"FourFour");

    kvsGet(1);
    sleep(1);
    kvsGet(4);
    sleep(1);
    kvsGet(22);
    sleep(1);
    kvsGet(33);
    sleep(1);
    kvsGet(11);
    sleep(1);
    kvsGet(4);
    sleep(1);
    kvsGet(2);
    sleep(1);
    kvsGet(3);
    sleep(1);
    kvsGet(44);
/*
    kvsDelete(1);
    kvsDelete(33);
    kvsDelete(4);
    kvsDelete(44);
    kvsGet(1);
    kvsGet(33);
    kvsGet(4);
    kvsGet(11);
    kvsGet(2);

    kvsPut(111,"OneOneOne");
    kvsPut(222,"TwoTwoTwo");
    kvsPut(333,"ThreeThreeThree");
    kvsPut(444,"FourFourFour");

    kvsGet(333);
    kvsGet(444);
    kvsGet(111);
    kvsGet(222);
*/
    close(clientFileDiscriptor);
   }
   else{
    printf("socket creation failed");
   }
   return 0;
  }

//    close(sockfd); //New

  //  return 0;
  //}


    void kvsPut(int key, char *value){
        int c = 1,l, p;
        char buffer1[256];
        bzero(buffer1, 256);
        printf("Key value\n");
        sprintf(buffer1,"%d:%d;%s", c,key, value);
  //      printf("\n** Key value 1**\n");
        l = write(clientFileDiscriptor,buffer1,strlen(buffer1));
  //      printf("\n**Key value 2: %s**\n",buffer1);
        if (l < 0)
        {
            error("ERROR writing to socket");
        }
          bzero(buffer1, 256);
          p = read(clientFileDiscriptor,buffer1,255);
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
      l = write(clientFileDiscriptor,buffer2,strlen(buffer2));
      if (l < 0)
      {
          error("ERROR writing to socket");
      }
      bzero(buffer4,256);
      p = read(clientFileDiscriptor,buffer4,255);
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
      l = write(clientFileDiscriptor,buffer3,strlen(buffer3));
      printf("Key: %d\n", key1);
      if (l < 0)
      {
          error("ERROR writing to socket");
      }
      bzero(buffer5,256);
      p = read(clientFileDiscriptor,buffer5,255);
      if (p < 0)
      {
          error("ERROR reading from socket");
      }
      printf("%s\n",buffer5);
    }
