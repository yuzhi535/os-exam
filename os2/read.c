#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFERSIZE 4096

int main(int argc, char *argv[])
{

    key_t shm_key;
    shm_key = ftok("read.c", 0);
    int shm_id = shmget(shm_key, 1024, 0666 | IPC_CREAT);
    if (shm_id == -1)
        printf("cread shmid fail!\n");

    char *shm_buf;

    // 得到共享内存地址
    if ((shm_buf = shmat(shm_id, 0, 0)) < (char *)0)
    {
        perror("shm_buf error\n");
        exit(1);
    }

    sem_t *mutex = sem_open("mutex", 1);
    sem_t *full = sem_open("full", 0);
    sem_t *empty = sem_open("empty", 1);
    int pid = fork();
    if (pid < 0)
    {
        perror("pid error\n");
    }

    else if (pid == 0)
    {
        while (1)
        {
            // 等待信号量，P操作
            sem_wait(full);
            sem_wait(mutex);

            printf ("son pid %d receive message:%s\n", getpid(), shm_buf);
            // 清空共享内存
            if (strncmp(shm_buf, "quit", 4) == 0)
            {
                sem_post(mutex);
                sem_post(empty);
                printf("子进程退出\n");
                break;
            }
            strcpy(shm_buf, "");

            // semaphore +1
            sem_post(mutex);
            sem_post(empty);
        } // while
    }     // else if
    else
    {
        while (1)
        {
            // 等待信号量，P操作
            sem_wait(full);
            sem_wait(mutex);

            printf("parent pid %d receive message:%s\n", getpid(), shm_buf);
            // 清空共享内存
            if (strncmp(shm_buf, "quit", 4) == 0)
            {
                sem_post(mutex);
                sem_post(empty);
                printf("父进程退出\n");
                break;
            }
            strcpy(shm_buf, "");

            // V操作
            sem_post(mutex);
            sem_post(empty);
        }
    }

    sem_close(mutex);
    sem_close(full);
    sem_close(empty);

    if (shmdt(shm_buf) == -1)
        printf("shmdt is fail\n");
    if (pid != 0)
    {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1)
            printf("shmctl delete error\n");
    }
    return 0;
}
