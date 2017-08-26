#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <list>
//#include "client_clock.h"
#include "../datagram/datagram.h"
#include <vector>
#include <sys/time.h>
#include <arpa/inet.h>

#ifndef UNTITLED_SERVER_H
#define UNTITLED_SERVER_H



using namespace std;

const int MESSAGE_FROM_GUI_LENGTH = 20;
const size_t CLIENT_DATAGRAM_NUMERIC_FIELDS_LENGTH = 13; //64 + 32 + 8 bits
const uint64_t two_secunds = 200000;
const uint64_t usecond = 1000000;

enum session_compare {current_session_lesser = 0, identical = 1, current_session_greater = 2};




void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//majac struct sockaddr their_addr;
//przekaz (struct sockaddr *)&their_addr
bool same_socket_addres(struct sockaddr *lhs, struct sockaddr *rhs) {
    if(lhs->sa_family == rhs->sa_family) {

        if (lhs->sa_family == AF_INET) {//IPv4
            if(!((struct sockaddr_in*)lhs)->sin_port == (((struct sockaddr_in*)rhs)->sin_port)){//port comparison
                return false;
            }
            return ((struct sockaddr_in*)lhs)->sin_addr.s_addr == ((struct sockaddr_in*)rhs)->sin_addr.s_addr//addres comparison
        }

        //IPv6
        if(!((struct sockaddr_in6*)lhs)->sin6_port == (((struct sockaddr_in6*)rhs)->sin6_port)){//port comparison
            return false;
        }
        char ip6_lhs[INET6_ADDRSTRLEN];
        char ip6_rhs[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, get_in_addr((struct sockaddr *)&lhs), ip6_lhs, sizeof ip6_lhs);
        inet_ntop(AF_INET6, get_in_addr((struct sockaddr *)&rhs), ip6_rhs, sizeof ip6_rhs);
        for(int i = 0; i < INET6_ADDRSTRLEN; i++) {
            if(ip6_lhs[i] != ip6_rhs[i])
                return false;
        }
        return true;
    }
    return false; //sa_family not match or nor IPv4 IPv6
}





class SERVER {

private:
    char datagram_buffer[DATAGRAM_SIZE + 1];

    /********** connected players and observers **********/
    //vector<CONNECTED_CLIENT> playing_clients;
    list<CONNECTED_CLIENT> playing_clients;
    list<CONNECTED_CLIENT> waiting_players_clients;
    list<CONNECTED_CLIENT> observers_clients;

    /************server settings*************/
    int8_t not_eliminated_players_number;
    uint32_t game_maxx, game_maxy;
    bool game_ongoing;
    uint64_t FPS;
    uint64_t round_time;



public:
    explicit SERVER();

    bool currently_game_ongoing() {return game_ongoing;}

    void disconnect_not_responding_clients();

    void next_round_start();

    void execute_next_round();

    void should_start_new_game();

    void receive_from_clients();

    void send_datagrams_to_client(); //parametr client
    void send_datagrams_to_all_players();

};


class EVENT {

public:
    vector<char> datagram_particle;
    int len;

explicit EVENT(int _len, vector<char> _datagram_particle) {
    len = _len;
    datagram_particle = _datagram_particle;
}

};


class CONNECTED_CLIENT {

public:
    int next_expected_event_no;
    string name;


private:
    struct sockaddr adr;
    int port;
    bool connected;
    uint64_t last_notification;
    uint64_t session_id;
    double x,y;
    double direction;
    struct timeval tval;
    int8_t player_number;

public:
    explicit CONNECTED_CLIENT(string _name, struct sockaddr _adr, uint64_t _session_id);
    void initialize_new_snake(double _x, double _y, double _direction) {x = _x; y = _y; direction = _direction;}
    void change_direction(double new_direction) {direction = new_direction;}
    void change_position(double new_x, double new_y) {x = new_x; y = new_y;}
    void set_player_number(int8_t number) {player_number = number;}
    bool is_connected() {return connected;}
    bool same_socket(struct sockaddr _adr) {return same_socket_addres((struct sockaddr *)&adr, (struct sockaddr *)&_adr);}

    session_compare same_session_id(uint64_t _session_id) {
        if(session_id < _session_id)
            return current_session_lesser;
        if(session_id > _session_id)
            return current_session_greater;
        return identical;
    }

    bool should_disconnect() {
        if(gettimeofday(&tval, NULL) == 0) {
            return last_notification + two_secunds <= (uint64_t)tval.tv_usec;
        }
        return false;
    }
    void disconnect() {connected = false;}
    void refresh_last_notification() {
        if(gettimeofday(&tval, NULL) == 0) {
            last_notification = (uint64_t)tval.tv_usec;
        }
    }
};



class RANDOM {

private:
    uint64_t r_0;
    uint64_t previous_rand;
    uint64_t return_rand;

public:
    explicit RANDOM(uint64_t seed = time(NULL)) {
        r_0 = seed;
        previous_rand = seed;
    }
    uint64_t next_rand() {return_rand = previous_rand;
        previous_rand = (previous_rand * 279470273) % 4294967291; return return_rand;}
    uint64_t previous_rand_rand() {return previous_rand;}
};



class ROUND_TIMER {

private:
    uint64_t time_milisec_since_last_update;
    struct timeval tval;
    uint64_t interval_in_milisec;

public:
    bool expired_last_time_interval() {
        if(gettimeofday(&tval, NULL) == 0) {
            if(tval.tv_usec - time_milisec_since_last_update >= interval_in_milisec) {
                time_milisec_since_last_update = (uint64_t)tval.tv_usec;
                return true;
            }
        }
        return false;
    }
    explicit ROUND_TIMER (uint64_t round_time) {interval_in_milisec = round_time;}

};



#endif //UNTITLED_SERVER_H

