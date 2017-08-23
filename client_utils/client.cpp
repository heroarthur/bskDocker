//class client     - tworzy klienta z odpowiednimi parametrami, client wysyla do gui i do servera

#include "client_clock.h"
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
#include "client.h"

using namespace std;


Client::Client (string name,
                const string& server_host, const string& server_port,
                const string& ui_host, const string& ui_port) {

    int8_t current_turn_direction = 0;
    uint32_t current_game_id = 0;
    uint32_t next_expected_event_no = 0;
    int64_t last_sent_event_number = -1;
    string player_name;
    uint64_t session_id = client_clock.get_session_id();
    game_server_host = server_host;
    game_server_port = server_port;
    game_ui_host = ui_host;
    game_ui_port = ui_port;
    datagram = Client_datagram();
    client_datagram_length = CLIENT_DATAGRAM_NUMERIC_FIELDS_LENGTH + player_name.length();

    current_players_names.push_back("karol");
    current_players_names.push_back("max");
    //current_players_names.push_back("torawareta");
    //current_players_names.push_back("idz_pan_do_domu");
}

bool Client::connect_to_server() {
    int rv;
    struct addrinfo *p;
    memset(&server_ints, 0, sizeof server_ints);
    //clear and set struct addrinfo
    server_ints.ai_family = AF_UNSPEC;
    server_ints.ai_socktype = SOCK_DGRAM;

    //getaddrinfo
    if ((rv = getaddrinfo(game_server_host.c_str(), game_server_port.c_str(), &server_ints, &servinfo)) != 0) {
        fprintf(stderr, "server getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }

    // loop through all the results and make a sockets for server
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((server_sockfd = socket(p->ai_family, p->ai_socktype,
                                    p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        if (connect(server_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(server_sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return false;
    }
    fcntl(server_sockfd, F_SETFL, O_NONBLOCK);
    return true;
}

bool Client::connect_to_gui() {
    int rv;
    int flag = 1;
    int err;

    memset(&gui_ints, 0, sizeof gui_ints);
    //clear and set struct addrinfo
    gui_ints.ai_family = AF_UNSPEC;
    gui_ints.ai_socktype = SOCK_STREAM;
    gui_ints.ai_protocol = IPPROTO_TCP;

    //getaddrinfo
    if ((rv = getaddrinfo(game_ui_host.c_str(), game_ui_port.c_str(), &gui_ints, &guiinfo)) != 0) {
        fprintf(stderr, "gui getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }
    // loop through all the results and make a sockets for server
    for(p = guiinfo; p != NULL; p = p->ai_next) {
        if ((gui_sockfd = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        if (connect(gui_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(gui_sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return false;
    }

    err = setsockopt(gui_sockfd,
                     IPPROTO_TCP,
                     TCP_NODELAY,
                     (char *) &flag,
                     sizeof(int));

    if (err < 0) {
        perror("NO_DELAY error");
        return false;
    }

    fcntl(gui_sockfd, F_SETFL, O_NONBLOCK);
    //send(gui_sockfd, "NEW_GAME 400 600 karol korwin \n", sizeof("NEW_GAME 400 600 karol korwin \n"), 0);
    return true;
}


bool Client::update_direction() {
    ssize_t rcv_len;
    string data;
    memset(gui_buffer, 0, MESSAGE_FROM_GUI_LENGTH);
    if ((rcv_len = recv(gui_sockfd, gui_buffer, MESSAGE_FROM_GUI_LENGTH, 0)) == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            //perror("socket now is empty");
            return true;
        }
        if (rcv_len < 0) {
            perror("read tcp");
            return false;
        }
    }
    if (rcv_len == 0) {
        perror("gui disconnected");
        return false;
    }
    data = string(gui_buffer);
    current_turn_direction = take_direction_from_gui_message(data);
    cout<<data<<endl;
    return true;
}


int8_t Client::take_direction_from_gui_message(const string& button_received) {
    string LEFT_BUTTON_DOWN = "LEFT_KEY_DOWN\n";
    string LEFT_BUTTON_UP = "LEFT_KEY_UP\n";
    string RIGHT_BUTTON_DOWN = "RIGHT_KEY_DOWN\n";
    string RIGHT_BUTTON_UP = "RIGHT_KEY_UP\n";

    if(LEFT_BUTTON_DOWN.compare(button_received) == 0) {
        return -1;
    }
    if(LEFT_BUTTON_UP.compare(button_received) == 0) {
        return 0;
    }
    if(RIGHT_BUTTON_DOWN.compare(button_received) == 0) {
        return 1;
    }
    if(RIGHT_BUTTON_UP.compare(button_received) == 0) {
        return 0;
    }
    return -2;
}


bool Client::should_sent_next_datagram() {
    return client_clock.expired_last_time_interval();
}

bool Client::send_datagram_to_server() {
    datagram.create_datagram_for_server(datagram_buffer,
                                        session_id, current_turn_direction,
                                        next_expected_event_no, player_name);

    if(send(server_sockfd, datagram_buffer, client_datagram_length, 0) == -1) {
        perror("send to server: ");
        return false;
    }
    return true;
}



bool Client::send_new_game_to_gui() {
    string message = "NEW_GAME " + to_string(maxx) + " " + to_string(maxy);

    for (auto i : current_players_names){
        message += " " + i;
    }
    message += "\n";
    cout<<message<<endl;
    //send(gui_sockfd, "NEW_GAME 400 600 karol korwin \n", sizeof("NEW_GAME 400 600 karol korwin \n"), 0);

    if(send(gui_sockfd, message.c_str(), message.length(), 0) == -1) {
        perror("send to gui NEW_GAME: ");
        return false;
    }
    return true;
}

bool Client::send_pixel_to_gui(const int &x, const int &y, const string &player_name) {
    string message = "PIXEL " + to_string(x) + " " + to_string(y) + " " + player_name + "\n";

    if(send(gui_sockfd, message.c_str(), message.length(), 0) == -1) {
        perror("send to gui PIXEL: ");
        return false;
    }
    return true;
}

bool Client::send_player_eliminate_to_gui(const string &player_name) {
    string message = "PLAYER_ELIMINATED " + player_name + "\n";
    cout<<"poszlo "<<endl;
    if(send(gui_sockfd, message.c_str(), message.length(), 0) == -1) {
        perror("send to gui PLAYER_ELIMINATED: ");
        return false;
    }
    return true;
}



bool Client::receive_datagram_from_server() {
    //rec
    //aplikuj zdarzenia po kolei
    return true;
}