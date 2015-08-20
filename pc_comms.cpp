#include "pc_comms.h"
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void sighandler(int signum, siginfo_t *info, void *ptr)
{

    //-- Extract the "this" pointer for the ParentChildComms class from the siginfo.
    ParentChildComms *this1 = (ParentChildComms *)info->si_value.sival_ptr;

    //-- not really supposed to print from signal handler context, but can get away with it 
    //-- for demo app
//    cout << "Received signal " << signum << endl ;
//    cout << "Signal originates from process " << (unsigned long)info->si_pid << endl ;

    this1->SetChildComplete ((unsigned long)info->si_pid);
}

void ParentChildComms::SetChildComplete (int pid) 
{

    //-- Iterate through the ChildData vector to find the child which just completed.
    //-- Then mark it complete.
    for (std::vector<ChildData*>::iterator it = childEndPoints.begin() ; it != childEndPoints.end(); ++it)
        if ( (*it)->pid == pid )
        {
            childCompleteBitField |= 1<<(*it)->childNum ;
        }
}

ParentChildComms::ParentChildComms (pid_t ppid ) :
parentPid (ppid)
{
    memset(&act, 0, sizeof(act));
    //-- Register signal handler
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &act, NULL);
}

void ParentChildComms::SendMessage ( int mq, message_buf *msg )
{
    int buf_length = sizeof (message_buf) ;

    /*
     * Send a message.
     */
    if (msgsnd(mq, msg, buf_length, 0 ) < 0) 
    {
       perror("msgsnd");
       exit(1);
    }
}

//-- Send an assignment to all children.
bool ParentChildComms::SendToChildren (message_buf *msg)
{
    //-- Iterate through the endpoint collection and send same message to each 
    //-- child (that's our architecture).
    childCompleteBitField = 0x0000; //-- initialize the indicator for completion ; 

    for (std::vector<ChildData*>::iterator it = childEndPoints.begin() ; it != childEndPoints.end(); ++it)
    {
//        cout << "sending message to: "<< (*it)->childNum  << endl ;
        SendMessage ((*it)->msgQueueId, msg) ;
    }

}

void ParentChildComms::AddChildCommEndpoint (int cn, int pid)
{
    int msgqid = 0 ;
    int msgflg = IPC_CREAT | 0666 ;

    if ((msgqid = msgget(queue_key, msgflg )) < 0) 
    {
        perror("msgget");
        exit(1); //-- TODO: really should throw exception here, probably, but point is we can't continue.
    }

    //-- Add a child communication endpoint to the collection
    ChildData * cd = new ChildData (cn, pid, msgqid ) ; 
    childEndPoints.push_back (cd) ;
}

//-- Have all children completed their assignment?
bool ParentChildComms::AllChildrenAckComplete ( )
{
    return childCompleteBitField == 0x3FF ; 
}
#define NANO_SECONDS_PER_SECOND 1000000000 
void ParentChildComms::SetMessagingPeriod (int msgPerSec)
{
  messagingPeriod = (int)((1.0/msgPerSec)*NANO_SECONDS_PER_SECOND) ;
}

bool ParentChildComms::WaitForCompletion (int timeout )
{
    struct timespec sleepTime ;
    if ( messagingPeriod < NANO_SECONDS_PER_SECOND )
    {
        sleepTime.tv_sec =  0;
        sleepTime.tv_nsec = messagingPeriod;
    }
    else
    {
        sleepTime.tv_sec = messagingPeriod/NANO_SECONDS_PER_SECOND ;
        sleepTime.tv_nsec = 0;
    }    
//    sleepTime.tv_nsec = messagingPeriod;

    int waittime = 0 ;

//    while ( waittime < (timeout*NANO_SECONDS_PER_SECOND) )
    {
         waittime++ ;

         //-- Ensure precise sleep time by handling EINTR (we
         //-- expect to be interrupted by signals)
         while (1)
         {
             /* Sleep for the time specified in tv. If interrupted by a
              * signal, place the remaining time left to sleep back into tv. */
             int rval = nanosleep (&sleepTime, &sleepTime);
             if (rval == 0)
             /* Completed the entire sleep time; all done. */
                 break;
             else if (errno == EINTR)
             /* Interrupted by a signal. Try again. */
                 continue;
             else 
             /* Some other error; bail out. */
                 break;
        }
        if ( AllChildrenAckComplete() )
            return true  ;
    }

    return false ;
}

ParentChildComms::ChildData::ChildData( int cn, int p, int mq ) :
childNum (cn),
pid (p),
msgQueueId (mq)
{

}
