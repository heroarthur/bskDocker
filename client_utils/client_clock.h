


#ifndef UNTITLED_CLIENT_CLOCK_H
#define UNTITLED_CLIENT_CLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <regex>
#include <sys/time.h>

class Clock {

public:
    uint64_t get_session_id();
    bool expired_last_time_interval();
    Clock ();

};


#endif //UNTITLED_CLIENT_CLOCK_H
