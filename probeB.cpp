#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "hub_def.h"

using namespace std;

int main()
{
	// Message buffer
	struct buf {// the ordering matters in this struct.
  		long mtype; // required
  		char greeting[50]; // mesg content
	};

	buf msg;  		// this is the container of the message.
	int size = sizeof(msg)-sizeof(long);         // calculate the size of the message.
	const int beta = 257; // seed value to determine valid data
	msg.mtype = 111;            // we are numbering our messages.

    int pass;                   // Holds whether the msgsnd succeeds
    int data;                   // holds data that will be sent to datahub
	int stop= 115; //signal from the hub to quit when the hub already recieve 10000 messages. 
	
	int qid = msgget(ftok(".",'u'), 0);// get locate the queue
    if(qid == -1){ // Exit out of program if the message queue is not succesful
        cout << strerror(errno) << endl;
        return -1;
    }
	
	//send the message to the hub
	while(msg.mtype != stop) {
		
		/* initialize random seed */
		srand(time(0)); 
		data = rand(); 
		if(data % beta == 0){
			const string spid = to_string((long)getpid());
			strncat(msg.greeting,spid.c_str(),size);
			/* 
			pass = msgrcv(qid, (struct msgbuf *)&msg, size, 116, 0); 
			if(pass == -1){
				cout << strerror(errno) <<endl; 
				return -1; 
			}  */
			
			
		}
	}
	
	return 0; 







}