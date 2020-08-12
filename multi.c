#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Size of the buffer
#define SIZE 10000

// Special marker used to indicate end of the producer data
#define END_MARKER -1

// Buffer, shared resource
char buffer[SIZE];
char buffer1[SIZE];
char buffer2[SIZE];
char buffer3[SIZE];
// Number of items in the buffer, shared resource
int end = 0;
int count = 0;
int count1 = 0;
int count2 = 0;
// Index where the producer will put the next item
int prod_idx = 0;
// Index where the consumer will pick up the next item
int con_idx = 0;
// How many items will be produced
int done = 1;
int done1 = 1;

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variables
pthread_cond_t cv_buffer1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_buffer2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_buffer3 = PTHREAD_COND_INITIALIZER;

/*
 Produce an item. Produces a random integer between [0, 1000] unless it is the last item to be produced in which case the value -1 is returned.
*/
char* produce()
{
    char *line = NULL;
    size_t size;
    // getline(&line, &size, stdin);
    //     if(strcmp(line, "DONE\n") == 0){
    //         exit(1);
    //     }
    //     else{
    //         strcpy(buffer, line);
    //         count = 1;
    //     }

    if(getline(&line, &size, stdin) == -1){
        done = 0;
    }
    else if(strcmp(line, "DONE\n")==0) {
        done = 0;
        exit(1);
    }
    else{
        strcpy(buffer, line);
        count = 1;
    }
    return buffer;
}

/*
 Function that the producer thread will run. Produce an item and put in the buffer only if there is space in the buffer. If the buffer is full, then wait until there is space in the buffer.
*/
void *producer(void *args)
{
    while(done != 0 || buffer != '\0'){
        pthread_mutex_lock(&mutex);
        while(count == 1){
            pthread_cond_wait(&cv_buffer1, &mutex);
        }
        produce();
        printf("%s\n", buffer);
        // Signal to the consumer that the buffer is no longer empty
        pthread_cond_signal(&cv_buffer1);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/*
 Get the next item from the buffer
*/
char *consume()
{
    int i;
    int size = sizeof(buffer);
    strcpy(buffer1, buffer);
    //memcpy(&buffer1, &buffer, size);
    for(i = 0; i < sizeof(buffer); i++){
        if(buffer1[i] == '\n'){
            buffer1[i] = ' ';
        }
    }
    count = 0;
    count1 = 1;
    return buffer1;
}

/*
 Function that the consumer thread will run. Get  an item from the buffer if the buffer is not empty. If the buffer is empty then wait until there is data in the buffer.
*/
void *consumer(void *args)
{
      // Lock the mutex before checking if the buffer has data  
    while(done1 != 0){
      pthread_mutex_lock(&mutex);
      while(count == 0){
        pthread_cond_wait(&cv_buffer1, &mutex);
      }
      consume();
      printf("%s\n", buffer1);
      // Signal to the producer that the buffer has space
      pthread_cond_signal(&cv_buffer2);
      //pthread_cond_signal(&cv_buffer1);
      // Unlock the mutex
      pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


char* consume1(){
    int size = sizeof(buffer1);
    int i, j;
    if(buffer1 == NULL){
        return;
    }
    for(i = 1; buffer1[i] != '\0'; i++){
        if (buffer1[i-1]=='+' && buffer1[i]=='+'){
            buffer1[i-1] = '^'; 
            for (j=i; buffer1[j]!='\0'; j++) 
                buffer1[j] = buffer1[j+1]; 
        }
    }
    memcpy(&buffer2, &buffer1, size);
    count1 = 0;
    count2 = 1;
    return buffer2;
}

void *consumer1(void *args){
    while(buffer1 != '\0'){
      // Lock the mutex before checking if the buffer has data      
      pthread_mutex_lock(&mutex1);
      while(count1 == 0){
          pthread_cond_wait(&cv_buffer2, &mutex1);
      }
      consume1();
      printf("%s\n", buffer2);
      // Signal to the producer that the buffer has space
      //pthread_cond_signal(&cv_buffer2);
      pthread_cond_signal(&cv_buffer3);
      // Unlock the mutex
      pthread_mutex_unlock(&mutex1);
    }
    return NULL;
}

char* consume2(){
    int counter = 0;
    int i;
    int size = sizeof(buffer2[0]) * 80;
    memcpy(&buffer3, &buffer2, size);
    count2 = 0;
    return buffer3;
}

void *consumer2(void *args){
    while(buffer2 != '\0'){
    pthread_mutex_lock(&mutex2);
    while(count2 == 0){
        pthread_cond_wait(&cv_buffer3, &mutex2);
    }
    consume2();
    printf("%s\n", buffer3);
    pthread_cond_signal(&cv_buffer1);
    pthread_cond_signal(&cv_buffer2);
    pthread_cond_signal(&cv_buffer3);
    pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}

int main(int argc, char *argv[])
{

    // Create the producer and consumer threads
    pthread_t p, c, c1, c2;
    pthread_create(&p, NULL, producer, NULL);
    // Sleep for a few seconds to allow the producer to fill up the buffer. This has been put in to demonstrate the the producer blocks when the buffer is full. Real-world systems won't have this sleep  
    sleep(5);  
    pthread_create(&c, NULL, consumer, NULL);

    sleep(5);
    pthread_create(&c1, NULL, consumer1, NULL);
    
    sleep(5);
    pthread_create(&c2, NULL, consumer2, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
    return 0;
}
