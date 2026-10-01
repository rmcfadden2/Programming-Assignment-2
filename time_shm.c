#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>

struct timeval *setup_shm();
void cleanup_shm(struct timeval *shm_ptr);

int main(int argc, char *argv[])
{
    struct timeval *shm_ptr = setup_shm();

    if (shm_ptr == NULL)
    {
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
        printf("Child process: PID = %d\n", getpid());
    }
    else
    {
        // Parent process
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        waitpid(pid, NULL, 0); // Wait for the child process to finish
        cleanup_shm(shm_ptr);
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

    struct timeval *shm_ptr = mmap(0, sizeof(struct timeval), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

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
