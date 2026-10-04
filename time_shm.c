#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
struct timeval *setup_shm();
void cleanup_shm(struct timeval *shm_ptr);

int main(int argc, char *argv[])
{
    // Require a command before creating shared memory.
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <command> [arguments...]\n", argv[0]);
        return 1;
    }

    struct timeval *shm_ptr = setup_shm();
    if (shm_ptr == NULL)
    {
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        cleanup_shm(shm_ptr);
        return 1;
    }
    else if (pid == 0)
    {
        // Record the starting timestamp in shared memory.
        if (gettimeofday(shm_ptr, NULL) == -1)
        {
            perror("gettimeofday failed");
            _exit(1);
        }

        // Execute the command with its command-line arguments.
        execvp(argv[1], &argv[1]);

        // Reached only if execvp() fails.
        perror("execvp failed");
        _exit(1);
    }
    else
    {
        int status;
        struct timeval end_time;

        // Wait for the child, retrying if interrupted by a signal.
        while (waitpid(pid, &status, 0) == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("waitpid failed");
            cleanup_shm(shm_ptr);
            return 1;
        }

        // Record the ending timestamp immediately after the child finishes.
        if (gettimeofday(&end_time, NULL) == -1)
        {
            perror("gettimeofday failed");
            cleanup_shm(shm_ptr);
            return 1;
        }

        // Calculate and print elapsed time here using *shm_ptr and end_time.

        cleanup_shm(shm_ptr);
        // Return the command's exit status if it exited normally.
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }

    return 0;
}

struct timeval *setup_shm()
{
    int shm_fd = shm_open("/time_shm", O_CREAT | O_RDWR, 0600);

    if (shm_fd < 0)
    {
        perror("shm_open failed");
        return NULL;
    }

    if (ftruncate(shm_fd, sizeof(struct timeval)) < 0)
    {
        perror("ftruncate failed");
        close(shm_fd);
        return NULL;
    }

    struct timeval *shm_ptr = mmap(
        NULL, sizeof(struct timeval),
        PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap failed");
        close(shm_fd);
        return NULL;
    }

    close(shm_fd);
    return shm_ptr;
}

void cleanup_shm(struct timeval *shm_ptr)
{
    if (shm_ptr != NULL)
    {
        munmap(shm_ptr, sizeof(struct timeval));
        shm_unlink("/time_shm");
    }
}