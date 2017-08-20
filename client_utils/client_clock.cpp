#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>
#include <iostream>


using namespace std;

class Clock {

private:
    uint64_t session_id;
    uint64_t time_milisec_since_last_update;
    struct timeval tval;
    uint64_t interval_in_milisec;
    void update_milisec ();

public:
    bool should_sent_next_datagram();
    uint64_t get_session_id();
    explicit Clock (uint64_t in_mili_sec);

};

Clock::Clock (uint64_t in_mili_sec) {
    interval_in_milisec = in_mili_sec;
    if(gettimeofday(&tval, NULL) == 0) {
        session_id = time_milisec_since_last_update = (uint64_t)tval.tv_usec;
    }
}

void Clock::update_milisec () {
    if(gettimeofday(&tval, NULL) == 0) {
        time_milisec_since_last_update = (uint64_t)tval.tv_usec;
    }
}

uint64_t Clock::get_session_id() {
    return session_id;
}

bool Clock::should_sent_next_datagram() {
    if(gettimeofday(&tval, NULL) == 0) {
        if(tval.tv_usec - time_milisec_since_last_update >= interval_in_milisec) {
            time_milisec_since_last_update = (uint64_t)tval.tv_usec;
            return true;
        }
    }
    return false;
}
