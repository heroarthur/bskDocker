#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <regex>
#include <iostream>
#include <stdlib.h>
#include <utility>
#include <string>
#include <arpa/inet.h>
#include <sstream>
#include <list>
#include <fcntl.h>
#include <sys/time.h>

using namespace std;

class Clock {

private:
    uint64_t session_id;
    uint64_t time_milisec_since_last_update;
    struct timeval tval;
    uint64_t interval_in_milisec;

public:
    void update_milisec ();
    bool should_sent_next_datagram();
    uint64_t get_session_id();
    Clock ();

};

void Clock::Clock () {
    if(gettimeofday(&tval, NULL) == 0) {
        session_id = time_milisec_since_last_update = tval.tv_usec;
    }
}

void Clock::update_milisec () {
    if(gettimeofday(&tval, NULL) == 0) {
        time_milisec_since_last_update = tval.tv_usec;
    }
}

uint64_t Clock::get_session_id() {
    return session_id;
}

bool Clock::should_sent_next_datagram() {
    if(gettimeofday(&tval, NULL) == 0) {
        if(tval.tv_usec - time_milisec_since_last_update >= interval_in_milisec) {
            time_milisec_since_last_update = tval.tv_usec;
            return true;
        }
    }
    return false;
}


