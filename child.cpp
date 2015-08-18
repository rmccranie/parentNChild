#include "child.h"
#include "utils.h"
#include <sys/prctl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include "pc_comms.h"
#include <cstdlib>

using namespace std;

Child::Child (int cn, int64_t pp) :
childNum (cn),
parentPointer (pp)
{

}

int Child::Run ()
{
    //-- Set process name to "Child"
    char mynameis[10];
    int msqid;
    message_buf  rbuf;

    snprintf (mynameis, sizeof(mynameis), "Child_%d", childNum);
    
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
    stringstream filename ;
    filename << Settings::getLogPath() << "/Child_" << childNum << "_log.txt" ;
    
    myFile.open (filename.str().c_str(), std::ofstream::out | std::ofstream::app);    

    myFile << Utils::currentDateTime() << ": I am the child: " << childNum << " logging to " << Settings::getLogPath() << endl ;

    if ((msqid = msgget(queue_key, 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    while ( 1 )
    {
        /*
         * Receive an answer of message type 1.
         */
        if (msgrcv(msqid, &rbuf, sizeof (message_buf), 1, 0) < 0) 
        {
            perror("msgrcv");
            exit(1);
        }

        myFile << Utils::currentDateTime() << ": message receieved - "
               << rbuf.num1 << ", " << rbuf.num2 << ": sum = " << rbuf.num1+rbuf.num2 << endl ;
    }
}
