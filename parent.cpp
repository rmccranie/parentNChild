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

Parent::Parent()
{
    children = 0;
    parentPid = getpid() ;
    pc_comms = new ParentChildComms () ;
    
    myFile << Utils::currentDateTime() << ": creating a parent..." << endl ;
    char mynameis[] = "Parent" ;
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
 

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
 
    cout << "parent pointer: " << parentPointer << endl ;
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
        Child *c = new Child(cn, parentPointer);
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
