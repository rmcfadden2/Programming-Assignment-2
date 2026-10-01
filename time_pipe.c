#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        // Child process
        printf("Child process: PID = %d\n", getpid());
    }
    else
    {
        // Parent process
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
    }

    return 0;
}