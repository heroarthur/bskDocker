#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <list>
//#include "client_clock.h"
#include "../datagram/datagram.h"
#include <vector>
#include <sys/time.h>
#include "server.h"

CONNECTED_CLIENT::CONNECTED_CLIENT(string _name, struct sockaddr _adr, uint64_t _session_id) {
    name = _name;
    adr = _adr;
    next_expected_event_no = 0;
    session_id = _session_id;
}


void SERVER::disconnect_not_responding_clients() {
    //disconnect playing clients
    for (list<CONNECTED_CLIENT>::iterator it=playing_clients.begin(); it!=playing_clients.end(); ++it) {
        if((*it).should_disconnect()) {
            it = observers_clients.erase(it);
            not_eliminated_players_number--;
            //if not_eliminated_players_number <= 1 game over
        }
    }
    //disconnect observers clients
    for (list<CONNECTED_CLIENT>::iterator it=observers_clients.begin(); it!=observers_clients.end(); ++it) {
        if((*it).should_disconnect()) {
            it = observers_clients.erase(it);
        }
    }
    //disconnect waiting players
    for (list<CONNECTED_CLIENT>::iterator it=waiting_players_clients.begin(); it!=waiting_players_clients.end(); ++it) {
        if((*it).should_disconnect()) {
            it = waiting_players_clients.erase(it);
        }
    }



}