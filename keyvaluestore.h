#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct kvs{
  int key;
  char name[256]; //  int value;
  struct kvs * next;
} kvs_t;

kvs_t * head = NULL;

kvs_t * getWrite(int k){

  kvs_t * current = head;

  while(current != NULL){
    if(current->key == k){
    return current;
    }
    current = current->next;
  }
  return NULL;
}

kvs_t * putWrite(int k, char * v){
  kvs_t * current = head;
  kvs_t * temp;
  int q;
  char num[256];

  temp = getWrite(k);
  sleep(5);
  if(temp == NULL){
    temp = (kvs_t *)malloc(sizeof(kvs_t));
//    printf("**6 MUTEX LOCK in put function by client: %d\n",c);
//    sleep(5); //lock
//    printf("**7 Key in Put function: %d %s by client: %d\n",k, v, c);
    temp->key = k;
    strcpy(temp->name, v);
    temp->next = head;
    head = temp;
  }
  else {
    strcpy(temp->name, v);
  }
  return temp;
}

int deleteWrite(int k){
  kvs_t * previous;
  kvs_t * current = head;

  if(current->key == k){
//    printf("**12 MUTEX LOCK in delete function by client: %d\n",c);
    head = current->next;
    free(current);
    return 1;
  }

  while(current != NULL){
    if(current->key == k){
//      pthread_mutex_lock(&gSharedMemoryLock);
      previous->next = current->next;
      free(current);
      return 1;
    }
    previous = current;
    current = current->next;
  }
  return 0;
}
