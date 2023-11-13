#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    for (int i = 0; i < 4; i++)
    {
        pid = fork();
    }
    printf("Hello, World!\n");
    return 0;
}