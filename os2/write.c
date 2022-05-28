#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define BUFFERSIZE 4096

int main(int argc, char *argv[])
{
    int shm_id;
    key_t shm_key;

    shm_key = ftok("read.c", 0);
    shm_id = shmget(shm_key, BUFFERSIZE, 0666 | IPC_CREAT);

    // create the share memory by shmget()
    if (shm_id < 0)
    {
        perror("shmid error\n");
        exit(1);
    } // if

    // get the virtual address of this share memory
    char *shm_buf;
    if ((shm_buf = shmat(shm_id, 0, 0)) < (char *)0)
    {
        perror("shmbuffer error!\n");
        exit(1);
    }

    // message input
    char message[1024];

    // create the semaphore
    sem_t *mutex = sem_open("mutex", O_CREAT, 0666, 1);
    sem_t *full = sem_open("full", O_CREAT, 0666, 0);
    sem_t *empty = sem_open("empty", O_CREAT, 0666, 1);

    char c;
    int i;
    printf("shmid = %d\n", shm_id);

    char path_in[100];
    FILE *read_file = NULL;
    while (!read_file)
    {
        printf("请输入写入文件地址：");
        scanf("%s", path_in);
        read_file = fopen(path_in, "r");
    }
    printf("文件打开成功！\n");

    while (1)
    {
        i = -1;

        int msg_len = fgets(message, 1024, read_file);
        if (!msg_len)
        {
            fclose(read_file);
            break;
        }

        // P操作
        sem_wait(empty);
        sem_wait(mutex);

        // 发送到共享内存
        sprintf(shm_buf, message);

        // V操作
        sem_post(mutex);
        sem_post(full);
    }

    printf("文件发送成功！\n");
    if (shmdt(shm_buf) == -1)
        printf("shmdt is fail\n");

    sem_close(mutex);
    sem_close(full);
    sem_close(empty);
    sem_unlink("mutex");
    sem_unlink("full");
    sem_unlink("empty");

    return 0;
}
