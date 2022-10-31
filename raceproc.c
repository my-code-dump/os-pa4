#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#define NUM 40

typedef struct moneymcpherson {
    int balance[2];
} Bank;

struct semaphores {
    sem_t mtx;
};

sem_t *test;
struct moneymcpherson *x; 
struct semaphores *s;

void* MakeTransactions() {
    int i, j, tmp1, tmp2, rint;
    double dummy;

    for (i = 0; i < 100; i++) {
        sem_wait(test);
        rint = (rand() % 30) - 15;
        if ((((tmp1 = x->balance[0]) + rint) >= 0) &&
                (((tmp2 = x->balance[1]) - rint) >= 0)) {
            x->balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            }
            x->balance[1] = tmp2 - rint;
        }
        sem_post(test);
    }
    return NULL;
}

int main (int argc, char** argv) {
    pid_t pid;
    srand(getpid());
    char* shm;
    size_t BANKSZ = sizeof(Bank);
    size_t SEMSZ = sizeof(sem_t);
    size_t MEMSZ = (BANKSZ + SEMSZ); 
    int shmid1, shmid2;

    /* ----- Memory segment for moneymcpherson ----- */
    key_t key1 = ftok("shmfile1",12343);
    // Create memory space segment
    if ((shmid1 = shmget(key1, 32, IPC_CREAT | 0666)) < 0) {
        perror("shmget1");
        exit(1);
    }

    // Attach it to our object to use
    if ((x = shmat(shmid1, NULL, 0)) == (struct moneymcpherson*) -1) {
        perror("shmat");
        exit(1);
    }

    // Setting up the balance since we are NOT using Bank
    x->balance[0] = 100;
    x->balance[1] = 100;

    /* ----- Memory segment for semaphores ----- */
    key_t key2 = ftok("shmfile2",87);
    // Create memory space segment
    if ((shmid2 = shmget(key2, 32, IPC_CREAT | 0666)) < 0) {
        perror("shmget2");
        exit(1);
    }

    // Attach it to our object to use
    if ((test = shmat(shmid2, NULL, 0)) == (sem_t*) -1) {
        perror("shmat");
        exit(1);
    }

    test = (sem_t*) (test + 1);
    sem_init(test, 1, 1);

    /* ----- Forking ----- */
    for (int i = 0; i < NUM; i++) { 
        printf("i = %d\n", i);
        printf("Init Balances A:%d + B:%d ==> %d!\n", 
                x->balance[0], x->balance[1], 
                x->balance[0] + x->balance[1]);

        switch (pid = fork()) {
            // Fork failures
            case -1:
                {
                    perror("Fork Failure");
                    exit(2);
                    break;
                }
            // Child Node
            case 0: 
                {
                    MakeTransactions();
                    exit(2);
                    break;
                }
            // Parent Node
            default:
                {
                    MakeTransactions();

                    sem_wait(test);
                                          fflush(stdout);
                       //wait(NULL);
                    sem_post(test);
                    break;
                }
            printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n", 
                      x->balance[0], x->balance[1], 
                      x->balance[0] + x->balance[1]);

        }
        /*
        printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n", 
                x->balance[0], x->balance[1], 
                x->balance[0] + x->balance[1]);
                */
    }
    sem_destroy(test);

    return 0;
}
