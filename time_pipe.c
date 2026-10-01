#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // The first element is going to be the read end and the second is write
    int pipe_fds[2];

    // Creates the pipe
    int pipeResult = pipe(pipe_fds);
    // Checks for creation
    if (pipeResult < 0) {
        perror("pipe creation failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        // Child process

        // Closes the read end of the pipe
        close(pipe_fds[0]);

        // Insert timestamp code here

        // Closes the write end of the pipe
        close(pipe_fds[1]);

        // Insert terminal code execution here

        // Can delete this if it runs on Windows
        printf("Child process: PID = %d\n", getpid());
    }
    else
    {
        // Parent process

        // Closes unused write end
        close(pipe_fds[1]);

        // Insert child-related code here

        // Closes the read end
        close(pipe_fds[0]);

        // Insert hand-off code here

        // Can delete this if it runs on Windows
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
    }

    return 0;
}