
#include <iostream>
#include "parent.h"
using namespace std;

int main ()
{
    Parent *p = new Parent() ;
    p->Run();
    cout << "Hello, World!" << endl ;

    return 0 ;
}

