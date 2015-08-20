#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include "pc_comms.h"

using namespace std ;

/*
 * Encapsulate app config settings.  Eventual idea would be to load these from
 * an ini file.
 */
class Settings
{
public:

    static string &getLogPath()  { return logPath ;  }
    static string &getIniFilename() { return iniFilename ; }
    static void setLogPath(string p)  { logPath = p ;  }
    static void setSendingRate(int rate) { sendingRate = rate ; }
    static void setComplexity (string c) { complexity = c ; }
    static int getSendingRate () { return sendingRate; }
    static OperationType getComplexity () ;
private:
    static string logPath ;
    static string iniFilename ;
    static int sendingRate ;
    static string complexity ;
};

#endif
