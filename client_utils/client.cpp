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



private:
    //bool try_rec_message_from_gui();

public:
    explicit Client (string name,
                     const string& game_server_host, const string& game_server_port,
                     const string& game_ui_host, const string& game_ui_port);
    bool should_sent_next_datagram();
    bool update_player_direction();
    bool connect_to_gui_server();
    bool test_getaddrinfo_other();
    bool connect_to_server();
    bool connect_to_gui();
    bool update_direction();
};

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
    cout<<"wartosci: "<<game_ui_host<<" "<<game_ui_port<<endl;
    if ((rv = getaddrinfo(game_ui_host.c_str(), game_ui_port.c_str(), &gui_ints, &guiinfo)) != 0) {
        fprintf(stderr, "gui getaddrinfo: %s\n", gai_strerror(rv));
        cout<<"no jhjh\n";
        return false;
    }
    cout<<"korwin"<<endl;
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
    send(gui_sockfd, "NEW_GAME 400 600 karol korwin \n", sizeof("NEW_GAME 400 600 karol korwin \n"), 0);
    return true;
}


bool Client::update_direction() {
    ssize_t rcv_len;
    string data;
    memset(buffer, 0, MESSAGE_FROM_GUI_LENGTH);
    if ((rcv_len = recv(gui_sockfd, buffer, MESSAGE_FROM_GUI_LENGTH, 0)) == -1) {
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
    data = string(buffer);
    current_turn_direction = take_direction_from_gui_message(data);
    cout<<(int)current_turn_direction<<" - kierunek\n";
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

