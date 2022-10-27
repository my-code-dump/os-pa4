#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct yoyotoys {
    int balance[2];
} Bank = {{100,100}};

void* MakeTransactions() {
    int i, j, tmp1, tmp2, rint;
    double dummy;

    for (i = 0; i < 100; i++) {
        rint = (rand() % 30) - 15;
        if ((((tmp1 = Bank.balance[0]) + rint) >= 0) &&
                (((tmp2 = Bank.balance[1]) - rint) >= 0)) {
            Bank.balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            }
            Bank.balance[1] = tmp2 - rint;
        }
    }
    return NULL;
}

//struct test *s;
//s->balance[0] = 50 // ==== (*s).balance[0]
                   //
int main (int argc, char** argv) {
    int i;
    void* voidptr = NULL;

        pthread_t tid[2];
        srand(getpid());

        printf("Init Balances A:%d + B:%d ==> %d!\n", 
                Bank.balance[0], Bank.balance[1], 
                Bank.balance[0] + Bank.balance[1]);

        int shmid;
        struct yoyotoys *s; 
        
        key_t key = ftok("schmfile",65);
        if ((shmid = shmget(key, 32, 0660)) < 0) {
            perror("schmget");
            exit(1);
        }
        
        if ((s = shmat(shmid, NULL, 0)) == (struct yoyotoys*) -1) {
            perror("shmat");
            exit(1);
        }
        
        printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n", 
                Bank.balance[0], Bank.balance[1], 
                Bank.balance[0] + Bank.balance[1]);
    return 0;
}
