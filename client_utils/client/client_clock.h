


#ifndef UNTITLED_CLIENT_CLOCK_H
#define UNTITLED_CLIENT_CLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <regex>
#include <sys/time.h>


const uint64_t CLIENT_TO_SERVER_DATAGRAM_INTERVAL = 20000; // 20ms


class Clock {

private:
    uint64_t session_id;
    uint64_t time_milisec_since_last_update;
    struct timeval tval;
    uint64_t interval_in_milisec;
    void update_milisec ();

public:
    bool expired_last_time_interval();
    uint64_t get_session_id();
    Clock ();

};


#endif //UNTITLED_CLIENT_CLOCK_H
