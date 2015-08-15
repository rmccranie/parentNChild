#include <iostream>
#include "parent.h"
#include "child.h"
#include <sys/wait.h>
#include <sys/prctl.h>
#include <ctime>
#include <cstring>

using namespace std;

void sighandler(int signum, siginfo_t *info, void *ptr)
{
    cout << "Received signal " << signum << endl ;
    cout << "Signal originates from process " << (unsigned long)info->si_pid << endl ;
}


Parent::Parent()
{
    children = 0;
    parentPid = getpid() ;
    
    memset(&act, 0, sizeof(act));
    cout << "creating a parent..." << endl ;
} 

bool Parent::IAmTheParent()
{
    return getpid() == parentPid ;
}

int Parent::CreateChild()
{
    if ( !IAmTheParent() )
        return 0 ;

    pid_t PID = 0;
    int result = 0, filedes[2], status;
    char mynameis[] = "Parent" ;
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
 
    //-- Register signal handler
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);

    /* Create child process: */
    PID = fork();

    if (PID == -1) {
        return 1;
    }

    result = pipe(filedes);

    if (PID != 0) {
        std::cout << "Created child process. PID: " << PID << "\n";
    }

    if (result == -1) {
        return 1;
    }

    if (PID == 0) { // This is the child process
        //std::string prog = "./" + path;
        cout << "Starting the child!" << endl ;
        Child *c = new Child();
        c->Run() ;
        //execvp(prog.c_str(), NULL); // Execute the program
    } else { // This is the parent process
//        waitpid(PID, &status, 0); // Wait for the child process to return.
        cout << "Process returned " << WEXITSTATUS(status) << ".\n";
        cout << "Press enter.\n";
    }

}

void Parent::CreateChildren()
{

    for ( int i = 0; i < numberOfChildren; i++ )
    {
        CreateChild();
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

    CreateChildren();

    do {
        wpid = waitpid(-1, &Stat, WNOHANG);
        if (wpid == 0) {
            if (waittime < timeout) 
            {
                cout << "Parent waiting " << waittime << " second(s)." << endl;
                waittime ++;
                nanosleep(&sleepTime, NULL); //should handle EINTR here... perhaps encapsulate sleep.
            }
            else {
                cout << "Killing child process" << endl ;
              //  kill(pid, SIGKILL); 
            }
        }
//    } while (wpid == 0 && waittime <= timeout);
    } while ( waittime <= timeout);
}
