#ifndef PARENT_H
#define PARENT_H

#include <iostream>
#include <csignal>
#include <fstream>
#include "settings.h"
#include "pc_comms.h"

using namespace std;

const int timeout = 180 ;
const int numberOfChildren = 10;

class Parent
{
public:
    Parent() ;
    int CreateChild (int) ;
    int Run();
private:
    int children ;
    pid_t parentPid ;
    ofstream myFile ;
    ParentChildComms * pc_comms ;
     //-- Private functions
    bool IAmTheParent() ;
    void CreateChildren() ;
};

#endif
