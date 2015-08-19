#include <iostream>
#include "parent.h"
#include "child.h"
#include <sys/wait.h>
#include <sys/prctl.h>
#include <ctime>
#include <cstring>
#include <sstream>
#include <utils.h>
#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <exception> //-- needed because boost lib call can throw exceptions
#include <inttypes.h>

using namespace std;
using namespace boost::filesystem;

bool Parent::running = true ;

Parent::Parent() 
{
    children = 0;
    parentPid = getpid() ;
    pc_comms = new ParentChildComms (parentPid) ;
    
    myFile << Utils::currentDateTime() << ": creating a parent..." << endl ;
    char mynameis[] = "Parent" ;
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
 
    memset(&act, 0, sizeof(act));
    //-- Register signal handler
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGHUP, &act, NULL);
} 

void Parent::sighandler(int signum, siginfo_t *info, void *ptr)
{

    //-- not really supposed to print from signal handler context, but can get away with it 
    //-- for demo app
//    cout << "Received signal " << signum << endl ;
//    cout << "Signal originates from process " << (unsigned long)info->si_pid << endl ;

    SetRunning (false) ;
}

bool Parent::IAmTheParent()
{
    return getpid() == parentPid ;
}

int Parent::CreateChild( int cn )
{
    int filedes[2], status;
    pid_t PID = 0;
    if ( !IAmTheParent() )
        return 0 ;

    //-- We are passing the comms pointer to each child to facilitate signal callbacks 
    //-- to object methods on pc_comms from children.
    int64_t parentPointer = (int64_t) pc_comms;
 
    /* Create child process: */
    PID = fork();

    if (PID == -1) {
        return 1;
    }

    if (PID != 0) {
        myFile << Utils::currentDateTime() << ": Created child process. PID: " << PID << "\n";
    }

    if (PID == 0) { // This is the child process
        //std::string prog = "./" + path;
        Child *c = new Child(cn, parentPointer, parentPid );
        c->Run() ;
    } 
    else 
    { // This is the parent process

    }

    return PID ;
}

void Parent::CreateChildren()
{
    for ( int i = 0; i < numberOfChildren; i++ )
    {
        int pid = CreateChild(i);
        pc_comms->AddChildCommEndpoint (i, pid) ;
    }
}
int Parent::Run()
{
    int waittime = 0;
  //  pid_t pid ;
    int Stat;
    int wpid;
    struct timespec sleepTime ;
    sleepTime.tv_sec = 1 ;
    sleepTime.tv_nsec = 0 ;
    stringstream filename ;

    //-- Use boost library, but should really do better error/exception 
    //-- handling here. For now, just bail.
    //--
    //-- TODO: do more sane exception handling.
    try {
        create_directories (Settings::getLogPath().c_str()) ;
    } catch (exception e ) {

        return -1 ;
    }
    
    filename << Settings::getLogPath() << "/Parent_log.txt" ;
    
    myFile.open (filename.str().c_str(), std::ofstream::out | std::ofstream::app);    

    CreateChildren(); if ( !IAmTheParent() ) return 0 ;

    message_buf message ;
    message.m_type = 1 ;
    message.num1 =  1234 ;
    message.num2 = 5678 ;
    message.cmd_type = start ; 
    message.op_type = simple ;

    //-- This is the main app loop. It sends messages and waits for 
    //-- responses from children.
    while (1)
    {
        myFile << Utils::currentDateTime() << ": Parent sending message: " 
               << message.num1 << ", " << message.num2 <<  endl;

        pc_comms->SendToChildren (&message) ;

        if (!pc_comms->WaitForCompletion (timeout) )
        {
            myFile << Utils::currentDateTime() << ": Timeline has been busted!!!" << endl ;
            break ;
        }   
        else if ( !running )
        {
            myFile << Utils::currentDateTime() << ": Shutting down children." << endl ;
            break ;
        } 
    } 
    
    //-- Send a stop message to children for orderly shutdown. 
    message.cmd_type = stop ; 
    pc_comms->SendToChildren (&message) ;

    return 0 ;
}
