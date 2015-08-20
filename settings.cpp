#include "settings.h"
#include "pc_comms.h"

string Settings::logPath = "./log";
string Settings::iniFilename = "./parentNChild.ini";
int Settings::sendingRate = 0 ;
string Settings::complexity = "simple" ;

OperationType Settings::getComplexity ()
{
    if ( complexity == "medium" ) return medium ;
    else if ( complexity == "hard" ) return hard;

    return simple ;
}
    
