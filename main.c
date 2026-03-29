#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#define MAX 1000000// MAX TICKETS
#define AGENT_BUFFER 10// number of agents = buffer array size

static volatile int counter = AGENT_BUFFER;
static volatile int buffer[AGENT_BUFFER];
static volatile int usePtr = 0;
static volatile int fillPtr = 0;

pthread_mutex_t lock;
pthread_cond_t empty, full;

//get a random value from the buffer array
int get() {
    int val = buffer[usePtr];
    usePtr = (usePtr + 1) % AGENT_BUFFER;
    counter--;// DONT UPDATE HERE, UPDATE IN AGENT CAUSE OF CHILDREN
    return val;
}

//put values into empty slots in buffer array
void put(int val) {
    buffer[fillPtr] = val;
    fillPtr         = (fillPtr + 1) % AGENT_BUFFER;

    counter++;
}

/**
 * Agent thread pulls value from buffer.
 * Determines age group & cost of ticket.
 * Updates counter if not child
 * Updates cost sum
 *
 * @param arg not used
 * @return NULL
 */
void * agent(void *arg)
{
    int i;
    int val;

    for(i=0;i<1000;i++){
        pthread_mutex_lock(&lock);

        while(counter == 0)
            pthread_cond_wait(&full, &lock);

        val = get();
        //if val <2 do not increment counter
        printf("Thread A%llu|Generating Ticket: %d\n",pthread_self(),val);
        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

/**
 * Producer thread generates random values between 0-100
 * Puts generated values into open spots of buffer array
 *
 * @param arg Not used
 * @return NULL
 */
void * producer(void *arg)
{
    for(int i = 0; i < MAX / AGENT_BUFFER; i++){
        int r = rand() % (101); // Random value between 0-100, rand % (MAX - MIN + 1) + MIN
        pthread_mutex_lock(&lock);
        while(counter == AGENT_BUFFER)
            pthread_cond_wait(&empty, &lock);

        put(r);
        printf("Thread p%llu|Producing: %d\n",pthread_self(),i);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}


int main(void) {

//while counter >0 keep threads alive?
    return 0;
}