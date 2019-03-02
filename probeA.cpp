#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>


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
  const int alpha = 997;                       // seed value to determine valid data
   msg.mtype = 111;            // we are numbering our messages.

    int pass;                   // Holds whether the msgsnd succeeds
    int data;                   // holds data that will be sent to datahub
	
	int qid = msgget(ftok(".",'u'), 0);          // get locate the queue
    if(qid == -1){                             // Exit out of program if the message queue is not succesful
        cout << strerror(errno) << endl;
        return -1;
    }


 
    do{
	 /* initialize random seed: */
		srand (time(0));
      data = rand();            // Generate the value

      if(data % alpha ==  0){   // Send message when a valid value is made
        const string spid = to_string((long)getpid());
        strncat(msg.greeting, spid.c_str(), size);// use strncat.

        pass = msgsnd(qid, (struct msgbuf *)&msg, size, 0);		// now we are sending the message.
        if(pass == 0){//successfully sent
          cout << "msg sent from A" << endl;
        }
		//receive acknowlegement from the hub
        pass = msgrcv (qid, (struct msgbuf *)&msg, size, 114, 0);
		if(pass > 0){
			 cout << "got a message back " << msg.greeting << endl;
			 break;
		}
		else {//waiting for the acknowlegement
			cout<< "waiting for acknowlegement from the data hub ...." <<endl;
			}
       
        
      }
    }
    while(data > 100);

    return 0;
}