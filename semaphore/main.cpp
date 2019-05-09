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
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <random>


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
    int userIn;
    int speed_check;
    int shmid;
    char *shmBUF;
    
    SEMAPHORE sem(2);//array of 2 semaphores
    
    //allocates shared memory
    shmid = shmget(IPC_PRIVATE, BUFFSIZE*sizeof(char), PERMS);
    //attach to shared memory
    shmBUF = (char *)shmat(shmid, 0, SHM_RND);
    
    cout << "Enter the number of time you want to do speed_check" <<endl;
    cin >> userIn;
    
    
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
    
//    for(int k=0; k<CHUNKSIZE; k++){
//        cout << *(shmBUF+k);
//    }
    
    default_random_engine generator(time(NULL) ^ (getpid()<<16));
    
    // define the range of the random number. (32 bit integer)
    uniform_int_distribution<int> distribution(-2147483648, 2147483647);
    
    int speedCheckCounter = 0;
    //create 5 processes
    for (int i=0; i< 4 ; i++){
        if (fork() == 0) {
            cout<<endl;
            printf("[child] pid %d from [parent] pid %d\n",getpid(),getppid());
            
            while (speedCheckCounter < userIn) {
                speedCheckCounter++;
                //Each process generate a random 32-bit random integer
                //create a random integer between 0 and 2^31 -1
                speed_check = distribution(generator);    // generate the 32 bit integer
                cout << "speed_check:" << speed_check<<endl;
                if (speed_check < 5000){
                    cout<<"!!!!!!!!"<<endl;
                    //randomly selects 2 groups and randomly chooses one chunk from each of
                    //these 2 groups to swap
                    int group1= rand()% 4 + 1;//select randomly for the 1st group to swap
                    int group2 = rand()% 4 + 1;//select randomly for the 2nd group
                    int chunk1 = rand()% 3 + 1;//select one chunk to swap
                    int chunk2 = rand() % 3 + 1;//select one chunk to swap
                    
                    while(group1 == group2){
                        group2 = rand() % 4 + 1;
                        cout <<group2<<endl;
                    }
//                    cout<<"group 1: "<<group1<<endl;
//                    cout<<"group 2: "<<group2<<endl;
//                    cout<<"chunk1: "<<chunk1<<endl;
//                    cout<<"chunk 2: "<<chunk2<<endl;
                    //need mutex here
                    swap(sem,shmBUF,group1,group2,chunk1,chunk2);
                    
                    //  swap(chunk1,chunk2);
                }
                else{
                    cout<<"speed_check is greater than 5000 -- no swap is made"<<endl;
                }
            }
            
            exit(0);
        }
    }
    
    for (int i = 0; i <5 ; i++ )
        wait(NULL);
    parent_cleanup(sem, shmid);
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
    
    // Get group1's starting index (offset1)
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
    
    // Get group2's starting index (offset2)
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
    
//    cout<<"offset1: "<<offset1<<endl;
//    cout<<"offset2: "<<offset2<<endl;
//
//
//    char temp1[CHUNKSIZE];
//    int index=0;
//    cout << "orginal"<<endl;
//    for(int k=offset1; k<end1; k++){
//        cout << *(shmBuf+k);
//    }
//
//    for(int i = offset1; i < end1; i++) {
//     temp1[index] = *(shmBuf+i);
//        index++;
//    }
//    //copy 2nd chunk into first chunk
//    int t_offset2 = offset2;
//    for(int i = offset1; i < end1; i++) {
//        *(shmBuf+i) = *(shmBuf+t_offset2);
//        t_offset2++;
//    }
//    index=0;
//    //copy 1st chunk into 2nd chunk
//    for(int i = offset2; i < end2; i++) {
//        *(shmBuf+i) = temp1[index];
//        index++;
//    }
//     cout << "copied"<<endl;
//    for(int k=offset2; k<end2; k++){
//        cout << *(shmBuf+k);
//    }
    
//    // swap contents of group1 chunk1 into group2 chunk2
//    char temp1[CHUNKSIZE];
//    char temp3[CHUNKSIZE];
//    cout << "second:";
//    for(int k=offset2; k<end2; k++){
//        cout << *(shmBuf+k);
//    }
//    cout<<endl;
//    int index = 0;
//    for (int i = offset1; i < end1; i++) {
//        temp1[index] = *(shmBuf + i);
//        index++;
//    }
//    char temp2[CHUNKSIZE];
//    index = 0;
//    for (int i = offset2; i < end2; i++) {
//        temp1[index] = *(shmBuf + i);
//        index++;
//    }
//    strncpy ( temp3, temp1, CHUNKSIZE );//copy first chunk to temp3
//    strncpy ( temp1, temp2, CHUNKSIZE );//copy 2nd chunk to 1st chunk
//    strncpy ( temp2, temp3, CHUNKSIZE );
//
//    cout << "first:";
//
//    for(int k=offset1; k<end1; k++){
//        cout << *(shmBuf+k);
//    }
//    cout<<endl;

    cout<<index<<endl;
    
}
void initializeLowerChars(char *shmBuf, int index){
    for(int i=index; i< CHUNKSIZE * 3; i++){
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


void parent_cleanup (SEMAPHORE &sem, int shmid) {
    
    int status;            /* child status */
    wait(0);    /* wait for child to exit */
    shmctl(shmid, IPC_RMID, NULL);    /* cleaning up */
    sem.remove();
} // parent_cleanup
