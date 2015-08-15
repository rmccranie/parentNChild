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
using namespace std;
using namespace boost::filesystem;

void sighandler(int signum, siginfo_t *info, void *ptr)
{
    //-- not really supposed to print from signal handler context, but can get away with it 
    //-- for demo app
    cout << "Received signal " << signum << endl ;
    cout << "Signal originates from process " << (unsigned long)info->si_pid << endl ;
}


Parent::Parent()
{
    children = 0;
    parentPid = getpid() ;
    
    memset(&act, 0, sizeof(act));
    myFile << Utils::currentDateTime() << ": creating a parent..." << endl ;
    char mynameis[] = "Parent" ;
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
 
    //-- Register signal handler
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);

} 

bool Parent::IAmTheParent()
{
    return getpid() == parentPid ;
}

int Parent::CreateChild( int cn )
{
    int result = 0, filedes[2], status;
    pid_t PID = 0;
    if ( !IAmTheParent() )
        return 0 ;

    /* Create child process: */
    PID = fork();

    if (PID == -1) {
        return 1;
    }

    result = pipe(filedes);

    if (PID != 0) {
        myFile << Utils::currentDateTime() << ": Created child process. PID: " << PID << "\n";
    }

    if (result == -1) {
        return 1;
    }

    if (PID == 0) { // This is the child process
        //std::string prog = "./" + path;
        Child *c = new Child(cn);
        c->Run() ;
    } 
    else 
    { // This is the parent process

    }

}

void Parent::CreateChildren()
{

    for ( int i = 0; i < numberOfChildren; i++ )
    {
        CreateChild(i);
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

    //-- Use boost library, but should really do better error/exception handling here.
    //-- TODO: create_directories may throw exception.
    create_directories (Settings::getLogPath().c_str()) ;

    filename << Settings::getLogPath() << "/Parent_log.txt" ;
    
    myFile.open (filename.str().c_str(), std::ofstream::out | std::ofstream::app);    

    CreateChildren(); if ( !IAmTheParent() ) return 0 ;
 
    do {
        wpid = waitpid(-1, &Stat, WNOHANG);
        if (wpid == 0) {
            if (waittime < timeout) 
            {
                myFile << Utils::currentDateTime() << "Parent waiting " << waittime << " second(s)." << endl;
                waittime ++;
                nanosleep(&sleepTime, NULL); //should handle EINTR here... perhaps encapsulate sleep.
            }
            else {
                myFile << Utils::currentDateTime() << "Killing child process" << endl ;
              //  kill(pid, SIGKILL); 
            }
        }
//    } while (wpid == 0 && waittime <= timeout);
    } while ( waittime <= timeout);

    return 0 ;
}
