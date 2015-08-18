#ifndef CHILD_H
#define CHILD_H

#include <iostream>
#include "settings.h"
#include <fstream>
#include <inttypes.h>

using namespace std ;

class Child
{

public: 
    Child(int, int64_t);
    int Run() ;

private:
    pid_t myPid ;
    int childNum;
    int64_t parentPointer ;
    ofstream myFile;
} ;

#endif
