
#include <iostream>
#include <csignal>

using namespace std;

const int timeout = 180 ;
const int numberOfChildren = 10;

class Parent
{
public:
    Parent() ;
    int CreateChild () ;
    int Run();
private:
    int children ;
    pid_t parentPid ;
    struct sigaction act;

     //-- Private functions
    bool IAmTheParent() ;
    void CreateChildren() ;
};
