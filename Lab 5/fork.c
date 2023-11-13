#include <stdio.h>
#include <unistd.h>

int main(void)
{
    __uint8_t pid;
    pid = fork();
    int value = 5;
    if (pid < 0)
    {
        printf("Fork failed");
    }
    else if (pid == 0)
    {
        value += 15;
        printf("Child: %d\n", value);
    }
    else
    {
        printf("Parent: %d\n", value);
    }

    printf("Value : %d\n", value);

    return 0;
}
