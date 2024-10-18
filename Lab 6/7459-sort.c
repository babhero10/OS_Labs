/**
 * Name: Abdullah Elsayed Ahmed
 * Description: Merge sort with threads.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/* Defined datatype */
typedef struct 
{
    int leftIndex;
    int rightIndex;
}
MergeSortArgs;

/* Global Variables */
int numberOfElements;
int *arr;

/* Functions deceleration */
void mergeSort(int leftIndex, int rightIndex);


/* Main code */
int main(void)
{
    // Get file name from the user
    char filename[100];
    fgets(filename, 100, stdin);
    filename[strlen(filename)-1] = '\0';

    // Reading input from file
    FILE *inputFile = fopen(filename, "r");
    if (inputFile == NULL)
    {
        perror("Can't find the file!\n");
        return 1;
    }
    
    fscanf(inputFile, "%d", &numberOfElements);

    arr = (int *)malloc(sizeof(int) * numberOfElements);

    for (int i = 0; i < numberOfElements; i++)
    {
        fscanf(inputFile, "%d", &arr[i]);
    }

    fclose(inputFile);

    mergeSort(0, numberOfElements - 1);

    for (int i = 0; i < numberOfElements; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");
    
    return 0;
}

/* Functions defintion */
void merge(int arr[], int leftIndex, int midIndex, int rightIndex)
{
    int leftArrLength = midIndex - leftIndex + 1;
    int rightArrLength = rightIndex - midIndex;
 
    // Arrays for merge
    int leftArr[leftArrLength];
    int rightArr[rightArrLength];
 
    // Store left array items
    for (int i = 0; i < leftArrLength; i++)
    {
        leftArr[i] = arr[leftIndex + i];
    }

    // Store right array items
    for (int i = 0; i < rightArrLength; i++)
    {
        rightArr[i] = arr[midIndex + 1 + i];
    }
 
    // Merge until one of them is empty
    int currentLeftIndex = 0;
    int currentRightIndex = 0;
    int currentIndex = leftIndex;

    while (currentLeftIndex < leftArrLength && currentRightIndex < rightArrLength)
    {
        if (leftArr[currentLeftIndex] <= rightArr[currentRightIndex])
        {
            arr[currentIndex] = leftArr[currentLeftIndex];
            currentLeftIndex++;
        }
        else
        {
            arr[currentIndex] = rightArr[currentRightIndex];
            currentRightIndex++;
        }

        currentIndex++;
    }
 
    // Remaining left array
    while (currentLeftIndex < leftArrLength) {
        arr[currentIndex] = leftArr[currentLeftIndex];
        currentLeftIndex++;
        currentIndex++;
    }
 
    // Remaining right array
    while (currentRightIndex < rightArrLength) {
        arr[currentIndex] = rightArr[currentRightIndex];
        currentRightIndex++;
        currentIndex++;
    }
}

void *mergeSortHelper(void *args)
{
    MergeSortArgs *mergeSortArgs = (MergeSortArgs *) args; 
    int rightIndex = mergeSortArgs->rightIndex;
    int leftIndex = mergeSortArgs->leftIndex;

    // st is of length 0 or 1 (Sorted)
    if (leftIndex >= rightIndex)
    {
        return NULL;
    }    

    pthread_t threadLeft, threadRight;
    int midIndex = (leftIndex + rightIndex) / 2;

    // Split
    MergeSortArgs *mergeSortArgsLeft = (MergeSortArgs *) malloc(sizeof(MergeSortArgs)); 
    
    // Arguments will be passed
    mergeSortArgsLeft->leftIndex = leftIndex;
    mergeSortArgsLeft->rightIndex = midIndex;
    
    pthread_create(&threadLeft, NULL, mergeSortHelper, mergeSortArgsLeft); // Sort left array

    MergeSortArgs *mergeSortArgsRight = (MergeSortArgs *) malloc(sizeof(MergeSortArgs)); 
    
    // Arguments will be passed
    mergeSortArgsRight->leftIndex = midIndex + 1;
    mergeSortArgsRight->rightIndex = rightIndex;

    pthread_create(&threadRight, NULL, mergeSortHelper, mergeSortArgsRight); // Sort right array

    // Wait for the left and the right array to be sorted
    pthread_join(threadLeft, NULL);
    free(mergeSortArgsLeft);

    pthread_join(threadRight, NULL);
    free(mergeSortArgsRight);

    // Merge
    merge(arr, leftIndex, midIndex, rightIndex);
}

void mergeSort(int leftIndex, int rightIndex)
{
    pthread_t mainThread;
    MergeSortArgs *mergeSortArgs = (MergeSortArgs *) malloc(sizeof(MergeSortArgs)); 
    
    // Arguments will be passed
    mergeSortArgs->leftIndex = leftIndex;
    mergeSortArgs->rightIndex = rightIndex;

    pthread_create(&mainThread, NULL, mergeSortHelper, mergeSortArgs);

    pthread_join(mainThread, NULL); // Wait until the array is sorted
}