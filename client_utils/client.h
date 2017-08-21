//
// Created by karol on 20.08.17.
//

#ifndef UNTITLED_CLIENT_H
#define UNTITLED_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <list>
#include "client_clock.h"


using namespace std;

const int MESSAGE_FROM_GUI_LENGTH = 20;


class Client {

private:
    int8_t current_turn_direction;
    uint32_t current_game_id;
    uint32_t next_expected_event_no;
    list<string> current_players_names; //cleared with new game, change during game
    int64_t last_sent_event_number;
    uint64_t session_id;
    string player_name;
    Clock client_clock = Clock();

private:
    string game_server_host, game_server_port;
    int server_sockfd;
    struct addrinfo server_ints, *servinfo;
    struct addrinfo *p;
    string game_ui_host, game_ui_port;
    int gui_sockfd;
    struct addrinfo gui_ints, *guiinfo;
    char buffer[MESSAGE_FROM_GUI_LENGTH];

private:
    int8_t take_direction_from_gui_message(const string& button_received);

public:
    explicit Client (string name,
                     const string& game_server_host, const string& game_server_port,
                     const string& game_ui_host, const string& game_ui_port);
    bool should_sent_next_datagram();
    bool connect_to_server();
    bool connect_to_gui();
    bool update_direction();
};


#endif //UNTITLED_CLIENT_H
