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

sem_t *mtx;
struct moneymcpherson *x; 

void* MakeTransactions() {
    int i, j, tmp1, tmp2, rint;
    double dummy;

    for (i = 0; i < 100; i++) {
        sem_wait(mtx);
        rint = (rand() % 30) - 15;
        if ((((tmp1 = x->balance[0]) + rint) >= 0) &&
                (((tmp2 = x->balance[1]) - rint) >= 0)) {
            x->balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            }
            x->balance[1] = tmp2 - rint;
        }
        sem_post(mtx);
    }
    return NULL;
}

/* ----- Memory segment for moneymcpherson ----- */
void createMoneyMemSpace() {
    int shmid1;
    key_t key1 = ftok("shmfile1",12);

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

    // Setting up the balance since we are NOT using Bank variable
    x->balance[0] = 100;
    x->balance[1] = 100;
}

/* ----- Memory segment for semaphores ----- */
void createSmphrMemSpace() {
    int shmid2;
    key_t key2 = ftok("shmfile2",87);

    // Create memory space segment
    if ((shmid2 = shmget(key2, 32, IPC_CREAT | 0666)) < 0) {
        perror("shmget2");
        exit(1);
    }

    // Attach it to our object to use
    if ((mtx = shmat(shmid2, NULL, 0)) == (sem_t*) -1) {
        perror("shmat");
        exit(1);
    }

    mtx = (sem_t*) (mtx + 1);
}

int main (int argc, char** argv) {
    pid_t pid;
    srand(getpid());

    createMoneyMemSpace(); 
    createSmphrMemSpace();
    
    sem_init(mtx, 1, 1);

    /* ----- Forking ----- */
    for (int i = 0; i < NUM; i++) { 
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
                    wait(NULL);
                    break;
                }
        }
        printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n\n", 
                x->balance[0], x->balance[1], 
                x->balance[0] + x->balance[1]);
    }

    sem_destroy(mtx);

    return 0;
}
