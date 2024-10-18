#include <iostream>
#include <queue>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* Macro definitions*/
#define NUMBER_OF_M_COUNTERS 5
#define MAX_SLEEPING_TIME 5
#define MAX_BUFFER_SIZE 5

/* Global variables */
int counter;
std::queue<int> buffer;

int rear, front;

pthread_mutex_t mutexBuffer;         // Mutex to lock buffer when be used from other thread.
sem_t semCounter, semBufferFull, semBufferEmpty; // Mutex to keep track of binary counter, produced and consumed data.


/* Functions declaration */
void *mCounter(void *arg);   // Thread increases counter variable.
void *mMonitor(void *arg);   // Thread read counter and write it into the buffer.
void *mCollector(void *arg); // Thread read the buffer.


int main(void)
{
    srand((unsigned int)time(NULL));             // To make random values.
    int mCounterThreadIDs[NUMBER_OF_M_COUNTERS]; // Store ids for the mCounterThreads

    // Variables for threads
    pthread_t mCounterThreads[NUMBER_OF_M_COUNTERS];    
    pthread_t mMonitorThreads; 
    pthread_t mCollectorThreads;

    // Initialize semaphore
    if (sem_init(&semCounter, 0, 1) == -1)
    {
        std::cerr << "Can't create semCounter!\n";
        return 1;
    }

    if (sem_init(&semBufferFull, 0, 0) == -1)
    {
        std::cerr << "Can't create semBufferFull!\n";
        return 1;
    }
    
    if (sem_init(&semBufferEmpty, 0, MAX_BUFFER_SIZE) == -1)
    {
        std::cerr << "Can't create semBufferEmpty!\n";
        return 1;
    }

    // Initialize mutex
    if (pthread_mutex_init(&mutexBuffer, NULL) != 0)
    {
        std::cerr << "Can't create mutexBuffer!\n";
        return 1;
    }

    // Initialize threads
    for (int i = 0; i < NUMBER_OF_M_COUNTERS; i++)
    {
        mCounterThreadIDs[i] = i + 1;

        if (pthread_create(&mCounterThreads[i], NULL, mCounter, &mCounterThreadIDs[i]))
        {
            std::cerr << "Can't create mCounterThreads" << i << "!\n";
            return 1;
        }
    }

    // if (pthread_create(&mMonitorThreads, NULL, mMonitor, NULL))
    // {
    //     std::cerr << "Can't create mMonitorThreads!\n";
    //     return 1;
    // }

    if (pthread_create(&mCollectorThreads, NULL, mCollector, NULL))
    {
        std::cerr << "Can't create mCollectorThreads!\n";
        return 1;
    }
    

    // Wait for threads
    for (int i = 0; i < NUMBER_OF_M_COUNTERS; i++)
    {
        if (pthread_join(mCounterThreads[i], NULL) != 0)
        {
            std::cerr << "Can't join mCounterThreads" << i << "!\n";
            return 2;
        }
    }

    if (pthread_join(mMonitorThreads, NULL) != 0)
    {
        std::cerr << "Can't join mMonitorThreads!\n";
        return 2;
    }

    if (pthread_join(mCollectorThreads, NULL) != 0)
    {
        std::cerr << "Can't join mCollectorThreads!\n";
        return 2;
    }

    // Free memory
    sem_destroy(&semCounter);
    sem_destroy(&semBufferEmpty);
    sem_destroy(&semBufferFull);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}


/* Functions definitions */
void *mCounter(void *arg)
{
    int threadID = *((int*)arg );

    while (1)
    {
        sleep(rand() % MAX_SLEEPING_TIME + 1);

        printf("Counter thread%d: received a message\n", threadID);
        printf("Counter thread%d: waiting to write\n", threadID);

        sem_wait(&semCounter);

        counter++;
        printf("Counter thread%d: now adding to counter, counter value=%d\n", threadID, counter);
        
        sem_post(&semCounter);
    }

    return NULL;
}

void *mMonitor(void *arg)
{
    int isEmpty, saveCounter, index;
    
    while (1)
    {
        printf("Monitor thread: waiting to read counter\n");

        sleep(rand() % MAX_SLEEPING_TIME + 1);
        
        saveCounter = counter;
        printf("Monitor thread: reading a count value of %d\n", saveCounter);

        counter = 0;

        sem_getvalue(&semBufferEmpty, &isEmpty);

        if (isEmpty == 0)
        {
            printf("Monitor thread: Buffer full!!\n");
        }
        
        sem_wait(&semBufferEmpty);
        sem_wait(&semCounter);
        pthread_mutex_lock(&mutexBuffer);
        
        buffer.push(saveCounter);
        printf("Monitor thread: writing to buffer at position %d\n", rear);
        rear = (rear + 1) % MAX_BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semCounter);
        sem_post(&semBufferFull);
    }
    
    return NULL;
}


void *mCollector(void *arg)
{
    int isFull, index;
    
    while (1)
    {
        sleep(rand() % MAX_SLEEPING_TIME + 1);

        sem_getvalue(&semBufferFull, &isFull);
        if (isFull == 0)
        {
            printf("Collector thread: nothing is in the buffer!\n");
        }
        
        sem_wait(&semBufferFull);
        pthread_mutex_lock(&mutexBuffer);
        
        buffer.pop();
        printf("Collector thread: reading from the buffer at position %d\n", front);
        front = (front + 1) % MAX_BUFFER_SIZE;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semBufferEmpty);
    }
    

    return NULL;
}

