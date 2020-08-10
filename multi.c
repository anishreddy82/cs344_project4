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
// Number of items in the buffer, shared resource
int count = 0;
// Index where the producer will put the next item
int prod_idx = 0;
// Index where the consumer will pick up the next item
int con_idx = 0;
// How many items will be produced
int num_iterations = 0;

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables
pthread_cond_t cv_buffer1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

/*
 Produce an item. Produces a random integer between [0, 1000] unless it is the last item to be produced in which case the value -1 is returned.
*/
char* produce()
{
    char *line = NULL;
    size_t size;
    if(getline(&line, &size, stdin) == -1){
        printf("No new line\n");
    }
    else{
        strcpy(buffer, line);
    }
    // printf(buffer);
    count = 1;
    return buffer;

}

/*
 Function that the producer thread will run. Produce an item and put in the buffer only if there is space in the buffer. If the buffer is full, then wait until there is space in the buffer.
*/
void *producer(void *args)
{
      printf("PROD %s\n", produce());
      // Signal to the consumer that the buffer is no longer empty
      pthread_cond_signal(&cv_buffer1);
      // Unlock the mutex
      pthread_mutex_unlock(&mutex);
    return NULL;
}

/*
 Get the next item from the buffer
*/
char *consume()
{
    int i;
    int size = sizeof(buffer);
    memcpy(&buffer1, &buffer, size);
    for(i = 0; i < sizeof(buffer); i++){
        if(buffer1[i] == '\n'){
            buffer1[i] = ' ';
        }
    }
    count = 0;
    return buffer1;
}

/*
 Function that the consumer thread will run. Get  an item from the buffer if the buffer is not empty. If the buffer is empty then wait until there is data in the buffer.
*/
void *consumer(void *args)
{
      // Lock the mutex before checking if the buffer has data      
      pthread_mutex_lock(&mutex);
      printf("CONS %s\n", consume());
      // Signal to the producer that the buffer has space
      pthread_cond_signal(&cv_buffer1);
      pthread_cond_signal(&cv_buffer2);
      // Unlock the mutex
      pthread_mutex_unlock(&mutex);
    return NULL;
}


char* consumer1(){
    char replace[] = "^";
    char toReplace[] = "++";
    int i;
    int size = sizeof(buffer);
    memcpy(&buffer2, &buffer1, size);
    for(i = 0; buffer2[i] != '\0'; i++){
        if(strstr(&buffer2[i], toReplace) == )
    }
}

void *consumer1(void *args){
      // Lock the mutex before checking if the buffer has data      
      pthread_mutex_lock(&mutex);

      printf("CONS %d\n", consume1());
      // Signal to the producer that the buffer has space
      pthread_cond_signal(&cv_buffer2);
      // Unlock the mutex
      pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[])
{

    // Create the producer and consumer threads
    pthread_t p, c, p1, c2;
    pthread_create(&p, NULL, producer, NULL);
    // Sleep for a few seconds to allow the producer to fill up the buffer. This has been put in to demonstrate the the producer blocks when the buffer is full. Real-world systems won't have this sleep  
    sleep(5);  
    pthread_create(&c, NULL, consumer, NULL);

    sleep(5);
    pthread_create(&c1, NULL, consumer1, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    pthread_join(c1, NULL);
    return 0;
}