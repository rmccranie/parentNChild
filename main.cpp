
#include <iostream>
#include "parent.h"
#include "utils.h"

using namespace std;

int main ()
{
    Parent *p = new Parent() ;
    Utils::loadIniFile() ;
    p->Run();

    return 0 ;
}

