#include "child.h"
#include <sys/prctl.h>

using namespace std;

Child::Child ()
{

    cout << "I am (really) the child! " << endl ;
}

int Child::Run ()
{
    char mynameis[] = "Child" ;
    prctl(PR_SET_NAME, (unsigned long) mynameis, 0, 0, 0);
    for ( int i = 0 ; i < 10; i++ )
    {
        cout << "and I'm running..." << endl ;
        sleep(1);
    }
}
