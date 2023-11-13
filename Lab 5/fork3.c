#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    __pid_t pid;
    pid = fork();
    int value = 5;

    if (pid < 0)
    {
        printf("Fork failed");
    }
    else if(pid == 0)
    {
        execlp("bin/ls", "ls", NULL);
        printf("Child: %d\n", value);
    }
    else
    {
        wait(NULL);
        printf("Child complete\n");
        printf("ParentLL %d\n", value);
    }

    return 0;
}