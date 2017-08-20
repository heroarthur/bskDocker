#include <string>
#include <sys/time.h>

#ifndef UNTITLED_CLIENT_CLOCK_H
#define UNTITLED_CLIENT_CLOCK_H


class Clock {

private:
    uint64_t session_id;
    uint64_t time_milisec_since_last_update;
    struct timeval tval;
    uint64_t interval_in_milisec;
    void update_milisec ();

public:
    uint64_t get_session_id();
    bool should_sent_next_datagram();
    explicit Clock (uint64_t in_mili_sec);

};


#endif //UNTITLED_CLIENT_CLOCK_H
