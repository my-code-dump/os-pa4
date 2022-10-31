#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct moneymcpherson {
    int balance[2];
} Bank = {{100,100}};

void* MakeTransactions(struct moneymcpherson *s, char origin[]) {
    int i, j, tmp1, tmp2, rint;
    double dummy;

    printf("%s\n", origin);

    for (i = 0; i < 100; i++) {
        rint = (rand() % 30) - 15;
        if ((((tmp1 = s->balance[0]) + rint) >= 0) &&
                (((tmp2 = s->balance[1]) - rint) >= 0)) {
            s->balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            }
            s->balance[1] = tmp2 - rint;
        }
    }
    return NULL;
}

void parent(){
    printf("Hello from parent\n");
}

void child(){
    printf("Hello from child\n");
}

int main (int argc, char** argv) {
    pid_t pid;
    int shmid;
    struct moneymcpherson *s; 

    key_t key = ftok("shmfile",12);

    // Create memory space segment
    if ((shmid = shmget(key, 32, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    // Attach it to our object to use
    if ((s = shmat(shmid, NULL, 0)) == (struct moneymcpherson*) -1) {
        perror("shmat");
        exit(1);
    }

    // Setting up the balance since we are NOT using Bank
    s->balance[0] = 100;
    s->balance[1] = 100;
    
    for (int i = 0; i < 40; i++) { 
        srand(getpid());

        printf("Init Balances A:%d + B:%d ==> %d!\n", 
                s->balance[0], s->balance[1], 
                s->balance[0] + s->balance[1]);

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
                    MakeTransactions(s,"Child");
                    exit(2);
                    break;
                }
            // Parent Node
            default:
                {
                    MakeTransactions(s,"Parent");
                    wait(NULL);
                    break;
                }
        }

        printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n", 
                s->balance[0], s->balance[1], 
                s->balance[0] + s->balance[1]);
    }

    return 0;
}
