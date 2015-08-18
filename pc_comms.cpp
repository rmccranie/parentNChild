#include "pc_comms.h"
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>


void sighandler(int signum, siginfo_t *info, void *ptr)
{

    //-- Extract the "this" pointer for the ParentChildComms class from the siginfo.
    ParentChildComms *this1 = (ParentChildComms *)info->si_value.sival_ptr;

    //-- not really supposed to print from signal handler context, but can get away with it 
    //-- for demo app
    cout << "Received signal " << signum << endl ;
    cout << "Signal originates from process " << (unsigned long)info->si_pid << endl ;

    this1->SetChildComplete ((unsigned long)info->si_pid);
}

void ParentChildComms::SetChildComplete (int pid) 
{

    for (std::vector<ChildData*>::iterator it = childEndPoints.begin() ; it != childEndPoints.end(); ++it)
        if ( (*it)->pid == pid )
        {
            childCompleteBitField |= 1<<(*it)->childNum ;
        }
}

ParentChildComms::ParentChildComms ()
{
    memset(&act, 0, sizeof(act));
    //-- Register signal handler
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &act, NULL);
}

void ParentChildComms::SendMessage ( int mq, message_buf *msg )
{
    /*
 *  *      * We'll send message type 1
 *   *           */
     
    int buf_length = sizeof (message_buf) ;

    /*
 *  *      * Send a message.
 *   *           */
    //if (msgsnd(mq, msg, buf_length, IPC_NOWAIT) < 0) 
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
    childCompleteBitField = 0x00; //-- initialize the indicator for completion ; 

    for (std::vector<ChildData*>::iterator it = childEndPoints.begin() ; it != childEndPoints.end(); ++it)
    {
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

//-- Have all childrent completed their assignment?
bool ParentChildComms::AllChildrenAckComplete ( )
{


}


ParentChildComms::ChildData::ChildData( int cn, int p, int mq ) :
childNum (cn),
pid (p),
msgQueueId (mq)
{

}
