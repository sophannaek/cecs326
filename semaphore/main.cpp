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

using namespace std;
const int CHUNKSIZE = 512; //the size of each chunk
const int numChunk =  3; //number of chunk in each segments
const int numGroup = 4 ; //number of segments/group
int BUFFSIZE = CHUNKSIZE * numChunk * numGroup;
enum{sem1,sem2};//semaphore variables

void swap(SEMAPHORE &sem, char *shmBuf,int num1, int num2);
void parent_cleanup(SEMAPHORE &, int);
void initializeLowerChars(char *shmBuf, int index);
void initializeUpperChars(char *shmBuf, int index);

int main(int argc, const char * argv[]) {
    // insert code here...
    int count;
    int speed_check;
    int shmid;
    char *shmBUF;

    SEMAPHORE sem(2);//array of 2 semaphores

    //allocates shared memory
    shmid = shmget(IPC_PRIVATE, BUFFSIZE*sizeof(char), PERMS);
    //attach to shared memory
    shmBUF = (char *)shmat(shmid, 0, SHM_RND);

    cout << "Enter the number of time you want to do speed_check" <<endl;
    cin >> count ;


    //create 4 groups/segments of shared memory
    //each group holds three 512 bytes chunk

    //the first group (all three chunks) will be initialized with lowercase letter
    //the 3 other group will be initialized with uppercase letter

    //create 5 processes
    for (int i=0; i< 5 ; i++){
        if (fork() == 0) {
            printf("[child] pid %d from [parent] pid %d\n",getpid(),getppid());

            //Each process generate a random 32-bit random integer
            //create a random integer between 0 and 2^31 -1
            srand(time(0));
            speed_check = rand();
            if (speed_check < 5000){
                //randomly selects 2 groups and randomy chooses one chunk from each of
                //these 2 groups to swap
                int num1= rand() % 4;
                int num2 = rand()% 4;
                int num3 = rand()% 3;
                int num4 = rand() %3;

                if(num1 ==num2){
                     int num2 = rand() % 4;
                }
                swap(sem1,shmBUF,num1,num2);



              //  swap(chunk1,chunk2);
            }

            exit(0);
        }
    }

    for (int i = 0l; i <5 ; i++ )
        wait(NULL);


    return 0;
}



void swap(SEMAPHORE &sem, char *shmBuf,int num1, int num2){
    
}
void initializeLowerChars(char *shmBuf, int index){
    for(int i=0; i< CHUNKSIZE; i++){
        //generate random lowercase letters
        char letter = (char)((rand()%26) + 97);
        *(shmBuf+i) = letter;
    }
}

void initializeUpperChars(char *shmBuf, int index){
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

