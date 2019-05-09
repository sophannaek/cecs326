//
//  main.cpp
//  speed_check
//

#include <iostream>
#include<ctime>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphore.h"
#include <cmath>


using namespace std;
const int CHUNKSIZE = 512; //the size of each chunk
const int numChunk =  3; //number of chunks in each group
const int numGroup = 4 ; //number of groups/segments of shared memory
int BUFFSIZE = CHUNKSIZE * numChunk;
enum{sem1, sem2};//semaphore variables

void swap(SEMAPHORE &sem, char *shmBuf,int num1, int num2);
void parent_cleanup(SEMAPHORE &, int);
void initializeLowerChars(char *shmBuf);
void initializeUpperChars(char *shmBuf);

int main(int argc, const char * argv[]) {
    // insert code here...
    int count;
    int speed_check;
    
    // 4 groups of shared memory:
    int shmid1;
    int shmid2;
    int shmid3;
    int shmid4;
    
    char *shmBUF1;
    char *shmBUF2;
    char *shmBUF3;
    char *shmBUF4;

    SEMAPHORE sem(2);//array of 2 semaphores


    cout << "Enter the number of times you want to do speed_check" <<endl;
    cin >> count;


    //create 4 groups/segments of shared memory:
    //each group holds three 512 byte chunks

    //allocates shared memory
    shmid1 = shmget(IPC_PRIVATE, BUFFSIZE, PERMS);
    //attach to shared memory
    shmBUF1 = (char *)shmat(shmid1, 0, SHM_RND);
    
    //allocates shared memory
    shmid2 = shmget(IPC_PRIVATE, BUFFSIZE, PERMS);
    //attach to shared memory
    shmBUF2 = (char *)shmat(shmid2, 0, SHM_RND);
    
    //allocates shared memory
    shmid3 = shmget(IPC_PRIVATE, BUFFSIZE, PERMS);
    //attach to shared memory
    shmBUF3 = (char *)shmat(shmid3, 0, SHM_RND);
    
    //allocates shared memory
    shmid4 = shmget(IPC_PRIVATE, BUFFSIZE, PERMS);
    //attach to shared memory
    shmBUF4 = (char *)shmat(shmid4, 0, SHM_RND);
    

    //the first group (all three chunks) will be initialized with lowercase letter
    initializeLowerChars(shmBUF1);
    
    //the 3 other group will be initialized with uppercase letter
    initializeUpperChars(shmBUF2);
    initializeUpperChars(shmBUF3);
    initializeUpperChars(shmBUF4);

    //create 5 processes
    for (int i=0; i< 5 ; i++){
        if (fork() == 0) {
            printf("[child] pid %d from [parent] pid %d\n",getpid(),getppid());

            //Each process generate a random 32-bit random integer
            //create a random integer between 0 and 2^31 -1
            srand(time(NULL));
            speed_check = rand();
            if (speed_check < 5000){
                //randomly selects 2 groups
                int num1 = rand() % 4;
                int num2 = rand() % 4;
                
                //randomly choose one chunk from each of these 2 groups to swap
                int num3 = rand() % 3;
                int num4 = rand() % 3;
                
                // get the offset for each chunk
                if(num3==1) {
                    int offset = 0;
                } else if(num3==2) {
                    int offset = 512;
                } else if(num3==3) {
                    int offset = 1024;
                }

                while (num1 == num2){ // If the 2 groups are the same
                     int num2 = rand() % 4;
                }
                
                
                swap(sem1, shmBUF, num1, num2);



              //  swap(chunk1,chunk2);
            }

            exit(0);
        }
    }

    for (int i = 0l; i <5 ; i++ )
        wait(NULL);


    return 0;
}



void swap(SEMAPHORE &sem, char *shmBuf, int num1, int num2){
    
}
void initializeLowerChars(char *shmBuf){
    for(int i=0; i < BUFFSIZE; i++){
        //generate random lowercase letters
        char letter = (char)((rand()%26) + 97);
        *(shmBuf+i) = letter;
    }
}

void initializeUpperChars(char *shmBuf){
    for(int i=0; i< CHUNKSIZE; i++){
        //generate random lowercase letters
        char letter = (char)((rand()%26) +65);
        *(shmBuf+i) = letter;
    }
}








void parent_cleanup (SEMAPHORE &sem, int shmid) {

    int status;            /* child status */
    wait(0);    /* wait for child to exit */
    shmctl(shmid, IPC_RMID, NULL);    /* cleaning up */
    sem.remove();
} // parent_cleanup

