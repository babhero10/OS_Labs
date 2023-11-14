#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char * usage = "Usage: cat_grep_output file-name word outfile\n";

const char *cat = "ls";
const char *grep = "wc";

int main(int argc, char **argv, char **envp)
{
    // if (argc < 4)
    // {
    //     fprintf(stderr, "%s", usage);
    //     exit(1);
    // }

    int numCommands = 2; // Number of commands in the array
    const char *commands[] = {cat, grep};

    int defaultin = dup(0);
    int defaultout = dup(1);
    int defaulterr = dup(2);

    int prevReadEnd = defaultin; // Initial input is from stdin
    int fdpipe[2];
    if (pipe(fdpipe) == -1)
    {
        perror("cat_grep: pipe");
        exit(2);
    }

    int nextWrite = fdpipe[1];

    for (int i = 0; i < numCommands; i++)
    {
        printf("%s\n", commands[i]);

        dup2(prevReadEnd, 0);
        dup2(nextWrite, 1);
        close(prevReadEnd);
        close(nextWrite);
        int pid = fork();
        if (pid == -1)
        {
            perror("cat_grep: fork");
            exit(2);
        }

        if (pid == 0)
        {
            // Child
            // Redirect err

            // Execute the current command in the array
            execlp(commands[i], commands[i], "-l", (char *)0);

            // exec() is not supposed to return, something went wrong
            perror("cat_grep: exec");
            exit(2);
        }

        // Parent
        // Save the read end of the current pipe for the next iteration
        prevReadEnd = fdpipe[0];
        if (i >= numCommands - 2) {
            nextWrite = defaultout;
        } else {
            nextWrite = fdpipe[1];
        }
    }

    // // Redirect output to the specified outfile
    // int outfd = creat(argv[3], 0666);

    // if (outfd < 0)
    // {
    //     perror("cat_grep: creat outfile");
    //     exit(2);
    // }

    // // Redirect input to the last read end
    // dup2(prevReadEnd, 0);

    // // Redirect output to the specified outfile
    // dup2(outfd, 1);
    // close(outfd);

    // // Redirect err
    // dup2(defaulterr, 2);

    // Close file descriptors that are not needed
    

    // Wait for the last command to finish
    waitpid(0, 0, 0);
    close(defaultin);
    close(defaultout);
    close(defaulterr);
    exit(2);
}
