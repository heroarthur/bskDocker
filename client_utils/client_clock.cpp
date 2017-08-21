#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <regex>
#include <sys/time.h>



using namespace std;

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

Clock::Clock () {
    interval_in_milisec = CLIENT_TO_SERVER_DATAGRAM_INTERVAL;
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

bool Clock::expired_last_time_interval() {
    if(gettimeofday(&tval, NULL) == 0) {
        if(tval.tv_usec - time_milisec_since_last_update >= interval_in_milisec) {
            time_milisec_since_last_update = (uint64_t)tval.tv_usec;
            return true;
        }
    }
    return false;
}
