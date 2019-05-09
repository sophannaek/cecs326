#include <iostream>
#include <ctime>
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
    //1st group from 0 to 512*3 - 1
    //2nd group from 512*3 to 512*6 - 1
    //3rd group from 512*6 to 512*9 - 1
    //4th group from 512*9 to 512*12 - 1

    initializeLowerChars(shmBUF, 0);//first group
    initializeUpperChars(shmBUF, 512*3);//2nd group
    initializeUpperChars(shmBUF, 512*6); //3rd group
    initializeUpperChars(shmBUF, 512*9);//4th group
    
    
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
                //randomly selects 2 groups and randomly chooses one chunk from each of
                //these 2 groups to swap
                int group1= rand()% 4;//select randomly for the 1st group to swap
                int group2 = rand()% 4;//select randomly for the 2nd group
                int chunk1 = rand()% 3;//select one chunk to swap
                int chunk2 = rand() % 3;//select one chunk to swap
                
                while(group1 == group2){
                    group2 = rand() % 4;
                    cout <<group2<<endl;
                }
                //need mutex here
                swap(sem,shmBUF,group1,group2,chunk1,chunk2);
                
                
                
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



void swap(SEMAPHORE &sem, char *shmBuf,int group1, int group2, int chunk1, int chunk2){
    cout <<"hello"<<endl;
    
    //1st group from 0 to 512*3 - 1
    //2nd group from 512*3 to 512*6 - 1
    //3rd group from 512*6 to 512*9 - 1
    //4th group from 512*9 to 512*12 - 1
    
    int offset1;
    int offset2;
    int end1;
    int end2;
    
    // Get group1's starting index
    if(group1==1) {
        offset1 = 0;
        // Get group1's ending index determined by what index chunk1 ends at
        if(chunk1==1) {
            end1 = 512-1;
        } else if(chunk1==2) {
            end1 = 512*2 - 1;
        } else if(chunk1==3) {
            end1 = 512*3 - 1;
        }
    } else if(group1==2) {
        offset1 = 512*3;
        // Get group1's ending index determined by what index chunk1 ends at
        if(chunk1==1) {
            end1 = 512*4 - 1;
        } else if(chunk1==2) {
            end1 = 512*5 - 1;
        } else if(chunk1==3) {
            end1 = 512*6 - 1;
        }
    } else if(group1==3) {
        offset1 = 512*6;
        // Get group1's ending index determined by what index chunk1 ends at
        if(chunk1==1) {
            end1 = 512*7 - 1;
        } else if(chunk1==2) {
            end1 = 512*8 - 1;
        } else if(chunk1==3) {
            end1 = 512*9 - 1;
        }
    } else if(group1==4) {
        offset1 = 512*9;
        // Get group1's ending index determined by what index chunk1 ends at
        if(chunk1==1) {
            end1 = 512*10 - 1;
        } else if(chunk1==2) {
            end1 = 512*11 - 1;
        } else if(chunk1==3) {
            end1 = 512*1 - 1;
        }
    }
    
    // Get group1's starting index
    if(group2==1) {
        offset2 = 0;
        // Get group1's ending index determined by what index chunk2 ends at
        if(chunk2==1) {
            end2 = 512-1;
        } else if(chunk2==2) {
            end2 = 512*2 - 1;
        } else if(chunk2==3) {
            end2 = 512*3 - 1;
        }
    } else if(group2==2) {
        offset2 = 512*3;
        // Get group1's ending index determined by what index chunk2 ends at
        if(chunk2==1) {
            end2 = 512*4 - 1;
        } else if(chunk1==2) {
            end2 = 512*5 - 1;
        } else if(chunk1==3) {
            end2 = 512*6 - 1;
        }
    } else if(group2==3) {
        offset2 = 512*6;
        // Get group1's ending index determined by what index chunk2 ends at
        if(chunk2==1) {
            end2 = 512*7 - 1;
        } else if(chunk1==2) {
            end2 = 512*8 - 1;
        } else if(chunk1==3) {
            end2 = 512*9 - 1;
        }
    } else if(group2==4) {
        offset2 = 512*9;
        // Get group1's ending index determined by what index chunk2 ends at
        if(chunk2==1) {
            end2 = 512*10 - 1;
        } else if(chunk1==2) {
            end2 = 512*11 - 1;
        } else if(chunk1==3) {
            end2 = 512*1 - 1;
        }
    }
    
    // swap contents of group1 chunk1 into group2 chunk2
    for (int i = offset1; i < end1; i++) {
        sem.P(sem1);
        tmp = *(shmBuf+i);
        sem.V(sem1);
    }
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
