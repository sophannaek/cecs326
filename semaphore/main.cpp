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

void swap(SEMAPHORE &, char *,int, int,int, int);
void parent_cleanup(SEMAPHORE &, int);
void initializeLowerChars(char *, int);
void initializeUpperChars(char *, int);

int main() {
    int count;
    signed int speed_check;
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
    //first group from 0 - 512*3
    initializeLowerChars(shmBUF,0);//first group
    initializeUpperChars(shmBUF,1);//2nd group
    initializeUpperChars(shmBUF, 2); //3rd group
    initializeUpperChars(shmBUF, 3);//4th group


    //create 5 processes
    for (int i=0; i< 5 ; i++){
        if (fork() == 0) {
            printf("[child] pid %d from [parent] pid %d\n",getpid(),getppid());

            //Each process generate a random 32-bit random integer
            //create a random integer between 0 and 2^31 -1
            srand(time(0));
            speed_check = rand();
            cout << speed_check<<endl;
            if (speed_check < 5000){
                //randomly selects 2 groups and randomy chooses one chunk from each of
                //these 2 groups to swap
                int num1= rand()% 4;//select randomly for the 1st group to swap
                int num2 = rand()% 4;//select randomly for the 2nd group
                int num3 = rand()% 3;//select one chunk to swap
                int num4 = rand() %3;//select one chunk to swap

                while(num1 ==num2){
                    num2 = rand() % 4;
                    cout <<num2<<endl;
                }
                //need mutex here
                swap(sem,shmBUF,num1,num2,num3,num4);



              //  swap(chunk1,chunk2);
            }
            else{
                cout<<"speed_check is greater than 5000 -- no swap is made"<<endl;
            }

            exit(0);
        }
    }

    for (int i = 0l; i <5 ; i++ )
        wait(NULL);

    return 0;
}



void swap(SEMAPHORE &sem, char *shmBuf,int num1, int num2, int num3, int num4){
    cout <<"hello"<<endl;
}
void initializeLowerChars(char *shmBuf, int index){
    for(int i=index; i< index + CHUNKSIZE * 3; i++){
        //generate random lowercase letters
        char letter = (char)((rand()%26) + 97);
        *(shmBuf+i) = letter;
    }
}

void initializeUpperChars(char *shmBuf, int index){
    for(int i=index; i< (index + CHUNKSIZE * 3); i++){
        //generate random lowercase letters
        char letter = (char)((rand()%26) +65);
        *(shmBuf+i) = letter;
    }
}
void create_group(SEMAPHORE &sem, char *shmBUF){

}

void parent_cleanup (SEMAPHORE &sem, int shmid) {

    int status;            /* child status */
    wait(0);    /* wait for child to exit */
    shmctl(shmid, IPC_RMID, NULL);    /* cleaning up */
    sem.remove();
} // parent_cleanup
