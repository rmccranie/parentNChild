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
#include <signal.h>

using namespace std;

Child::Child (int cn, int64_t pp, int ppid) :
childNum (cn),
parentPointer (pp),
parentPid (ppid)
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
        exit(1); //-- TODO: really needs to throw an exception
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

        struct timespec sleepTime ;
        sleepTime.tv_sec = 0 ;
        sleepTime.tv_nsec = operation_delay_ns[rbuf.op_type] ;

        if ( rbuf.cmd_type == stop )
            break ; 
        //-- Perform the computation requested
        myFile << Utils::currentDateTime() << ": message receieved - "
               << rbuf.num1 << ", " << rbuf.num2 << ": sum = " << rbuf.num1+rbuf.num2 << endl ;

        //-- I'm using sleep time to emulate computational complexity.
        //-- This could be done in a number of ways (such selecting different
        //-- math operations to do with the numbers), but delay is a pretty 
        //-- decent analog.
        nanosleep (&sleepTime, NULL ) ;

        union sigval value ;

        value.sival_ptr = (void*) parentPointer ;

        myFile << Utils::currentDateTime() << ": operation complete!" << endl ;

        //-- Signal
        int ret = sigqueue ( parentPid, SIGRTMIN, value ) ;
    }
}
