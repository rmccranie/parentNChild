#ifndef CHILD_H
#define CHILD_H

#include <iostream>
#include "settings.h"
#include <fstream>

using namespace std ;

class Child
{

public: 
    Child(int);
    int Run() ;

private:
    pid_t myPid ;
    int childNum;
    ofstream myFile;
} ;

#endif
