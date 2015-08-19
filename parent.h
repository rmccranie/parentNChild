#ifndef PARENT_H
#define PARENT_H

#include <iostream>
#include <csignal>
#include <fstream>
#include "settings.h"
#include "pc_comms.h"

using namespace std;

const int timeout = 60 ;
const int numberOfChildren = 10;

class Parent
{
public:
    Parent() ;
    int CreateChild (int) ;
    int Run();
    static bool SetRunning (bool set) { running = set ; }
private:
    int children ;
    pid_t parentPid ;
    ofstream myFile ;
    ParentChildComms * pc_comms ;
    static bool running ;
     //-- Private functions
    bool IAmTheParent() ;
    void CreateChildren() ;
    struct sigaction act;
    static void sighandler (int, siginfo_t *, void * ) ; 
};

#endif
