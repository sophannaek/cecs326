#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>
#include "hub_def.h"


/*
 * This files executes the datahub which takes input from
 * Probe A, Probe B, and Probe C. It then ouputs the Process Id of the probe
 * and the data that the probe sent.
*/
using namespace std;

int main()
{
	struct buf{
		long mtype;
		char greeting[50]; //msg content
	};
	
	buf msg; //declare container for the message
	int size = sizeof(msg) - sizeof(long);//calculate the size of the message. 
	
	
	   // while any of the probes are on keep checking the messages
    //they will be false when we terminate the probe
    bool flag_a = true;
    bool flag_b = true;
    bool flag_c = true;


    // variables that store the process id of each probe
    string Apid = "";
    string Bpid = "";
    string Cpid = "";

    // constants to hold the qid of each probe messages
    const int Amtype = 111;
    const int Bmtype = 112;
    const int Cmtype = 113;
	
	//variable to track whether a msgrcv is successful 
	int pass = 0; 
	int count=0; //count the number of messages that the hub receivs 
	int stop= 115; //signal from the hub to quit when the hub already recieve 10000 messages. 
	
	
	
	
	//create the message queue  
	int qid = msgget(ftok(".", 'u'), IPC_EXCL | IPC_CREAT | 0600); 
	//if the queue is not succesful created
	if(qid == -1){
		cout <<strerror(errno) << endl; 
		return -1; 
	}
	
	
	//keep check message, while probes are still on
	while(flag_a || flag_b, flag_c)
	{	
		//check if msg is sent from probeA 
		if(msg.mtype == Amtype){
			pass = msgrcv(qid, (struct msgbuf *)&msg, size, Amtype,0);
			if(pass !=1)//sucessful read from A
			{
				cout <<"reading message from A.. " <<endl; 
				Apid = msg.greeting; 
				cout << Apid <<endl; 
				msg.type= 114; 
				strncat(msg.greeting, "acknowledge");
				msgsnd(qid,(struct msgbuf *)&msg, size, 0); 
				count++;
				if(Apid < 100){
					flag_a = false; //terminate the probe --> call exit on A 
				}
				
			}
		}
		else if(msg.mtype == Bmtype){
			pass = msgrcv(qid, (struct msgbuf *)&msg, size, Bmtype,0);
			if(pass !=1){
				cout<< "reading message from B... "<<endl;
				Bpid = msg.greeting;
				count++;
				if(count >= 10000){ 
					flag_B = false;//probeB is forced to terminate
					msg.type = stop;
					strncat(msg.greeting, "The hub has received morethan 10000 messages! " );
					msgsnd(qid,(struct msgbuf *)&msg, size, 0);
				}
			}
		}
		else{//msg sent from probe C
			pass = msgrcv(qid, (struct msgbuf *)&msg, size, Cmtype,0);
			if(pass !=1){
				cout<< "reading message from B... "<<endl;
				Bpid = msg.greeting;
				count++;
				
			}
		}
	}
	
	  msgctl (qid, IPC_RMID, NULL);// this deallocates the queue.

    return 0;
	
}