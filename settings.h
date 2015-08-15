#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

using namespace std ;

class Settings
{
public:

    static string &getLogPath() { return logPath ;}

private:
    static string logPath ;

};

#endif
