#include "child.h"
#include "utils.h"
#include <sys/prctl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>

using namespace std;

Child::Child (int cn, int64_t pp) :
childNum (cn),
parentPointer (pp)
{
    cout << "parent pointer (in child): " << pp << endl ;
}

int Child::Run ()
{
    //-- Set process name to "Child"
    char mynameis[10];
    snprintf (mynameis, sizeof(mynameis), "Child_%d", childNum);
    
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
    stringstream filename ;
    filename << Settings::getLogPath() << "/Child_" << childNum << "_log.txt" ;
    
    myFile.open (filename.str().c_str(), std::ofstream::out | std::ofstream::app);    

    myFile << Utils::currentDateTime() << ": I am the child: " << childNum << " logging to " << Settings::getLogPath() << endl ;
    while ( 1 )
    {
        myFile << Utils::currentDateTime() << ": and I'm running..." << endl ;
        sleep(1);
    }
}
