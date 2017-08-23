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
#include "datagram.h"


using namespace std;

const int MESSAGE_FROM_GUI_LENGTH = 20;
const size_t CLIENT_DATAGRAM_NUMERIC_FIELDS_LENGTH = 13; //64 + 32 + 8 bits

class Client {

private:
    int8_t current_turn_direction;
    uint32_t current_game_id;
    uint32_t next_expected_event_no;
    int64_t last_sent_event_number;
    uint64_t session_id;
    string player_name;
    Clock client_clock = Clock();
    Client_datagram datagram;
    size_t client_datagram_length;

    uint32_t maxx = 500, maxy = 500;
    list<string> current_players_names; //cleared with new game, change during game




private:
    string game_server_host, game_server_port;
    int server_sockfd;
    struct addrinfo server_ints, *servinfo;
    struct addrinfo *p;
    string game_ui_host, game_ui_port;
    int gui_sockfd;
    struct addrinfo gui_ints, *guiinfo;

    char gui_buffer[MESSAGE_FROM_GUI_LENGTH];
    ssize_t recv_length;

    //int next_free_byte;

private:
    int8_t take_direction_from_gui_message(const string& button_received);

public:
    char datagram_buffer[DATAGRAM_SIZE+1];



public:
    //bool try_rec_message_from_gui();
    bool send_new_game_to_gui();
    bool send_pixel_to_gui(const int &x, const int &y, const string &player_name);
    bool send_player_eliminate_to_gui(const string &player_name);


public:
    explicit Client (string name,
                     const string& game_server_host, const string& game_server_port,
                     const string& game_ui_host, const string& game_ui_port);
    bool should_sent_next_datagram();

    bool connect_to_server();
    bool connect_to_gui();

    bool update_direction();
    bool send_datagram_to_server();
    bool receive_datagram_from_server();



};

#endif //UNTITLED_CLIENT_H
