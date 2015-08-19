#ifndef PC_COMMS_H
#define PC_COMMS_H

#include <vector>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <inttypes.h>

using namespace std;

#define MSGSZ     128

//-- Define message structure.
typedef enum CommandType   { start, stop, quit }    CommandType;
typedef enum OperationType { simple, medium, hard } OperationType;

typedef struct message_buf
{
    long          m_type;
    OperationType op_type;
    CommandType   cmd_type;
    int           num1;
    int           num2;
    char          mtext[MSGSZ];
} message_buf;


const key_t queue_key = 1234 ;
const int operation_delay_ns [3] = { 10000000, 100000000, 500000000 } ;
class ParentChildComms
{
public:
    ParentChildComms (pid_t) ;
    //-- This is the broadcast method to message all children.
    bool SendToChildren ( message_buf* );
    void AddChildCommEndpoint (int, int);
    bool AllChildrenAckComplete () ;
    void SetChildComplete (int) ; 
    bool WaitForCompletion (int) ;
    void SetMessagingPeriod (int);
private:
    void SendMessage ( int, message_buf* ) ;
    //-- Encapsulate data needed to communicate with a child.
    class ChildData
    {
    public:
        ChildData (int, int, int) ;
        int GetChildNum ()  { return childNum;   }
        int GetPid()        { return pid;        }
        int GetMsgQueueId() { return msgQueueId; }

        int childNum ;
        int pid;
        int msgQueueId ;
    } ; 
    
    struct sigaction act;
    vector<ChildData *> childEndPoints ;
    //-- an unsigned char will do to hold this for now.
    int16_t childCompleteBitField ;
    pid_t parentPid ;
    int messagingPeriod; 
} ;

#endif
