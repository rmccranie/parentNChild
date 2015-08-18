#include "utils.h"
#include <sys/time.h>
#include <cstdio>

const std::string Utils::currentDateTime() {

    char            fmt[64], buf[80];
    struct timeval  tv;

    struct tm*  tstruct;

    gettimeofday(&tv, NULL);
    if((tstruct = localtime(&tv.tv_sec)) != NULL)
    {
        strftime(fmt, sizeof fmt, "%Y-%m-%d %H:%M:%S.%%06u", tstruct);
        snprintf(buf, sizeof buf, fmt, tv.tv_usec);
    }

    return buf;
}


