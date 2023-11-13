/**
 * Number of process = 2^(number_of_forks)
*/
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    __pid_t pid;
    pid = fork();

    pid = fork();
    
    pid = fork();
    printf("Hello, World!\n");
    return 0;
}