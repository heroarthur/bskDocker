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
    client_datagram = Client_datagram();
    client_datagram_length = CLIENT_DATAGRAM_NUMERIC_FIELDS_LENGTH + player_name.length();

    current_game_name_list.push_back("karol");
    current_game_name_list.push_back("max");

    //game_logic
    next_expected_event_no = 0;
    game_continues = false;
    game_over = false;
    not_eliminated_players_number = number_of_players = 0;
    current_game_id = 0;
    maxx = 400;
    maxy = 400;
    current_game_name_list.clear();
    current_players_names_arr.clear();
    players_not_eliminated.clear();
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
    client_datagram.create_datagram_for_server(datagram_buffer,
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

    for (auto i : current_game_name_list){
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
    cout<<message<<endl;
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
    if ((recv_length = recv(server_sockfd, datagram_buffer, DATAGRAM_SIZE, 0)) == -1) {
        perror("recv from server");
        return false;
    }

    client_datagram.clear_datagram(datagram_buffer);
    client_datagram.set_recv_length(recv_length);
    client_datagram.get_game_id(datagram_buffer, recv_game_id);

    if(recv_game_id != current_game_id) {
        if(recv_game_id == finished_game_id) return end_analize_datagram;
        if(!client_datagram.datagram_starts_with_new_game(datagram_buffer)) return end_analize_datagram;
    }

    while(client_datagram.get_next_free_byte() <= recv_length-1) {

        if(!client_datagram.get_next_event_length(datagram_buffer, tmp_event_len)) return end_client_program;
        if(!client_datagram.event_checksum_correct(datagram_buffer, tmp_event_len)) return end_analize_datagram;

        client_datagram.get_event_no(datagram_buffer, recv_event_no);
        client_datagram.get_next_event_type(datagram_buffer, recv_event_type);
        if(!try_apply_event())
            return end_client_program;

    }
    return end_analize_datagram;
}


bool Client::receive_datagram_from_server_test(const char* datagram, int recv_len_test) {


    client_datagram.clear_datagram(datagram_buffer);

    mempcpy(datagram_buffer, datagram, recv_len_test);//test

    client_datagram.set_recv_length(recv_len_test);
    client_datagram.get_game_id(datagram_buffer, recv_game_id);

    if(recv_game_id != current_game_id) {
        cout<<"zle game id\n";
        if(recv_game_id == finished_game_id) return end_analize_datagram;
        if(!client_datagram.datagram_starts_with_new_game(datagram_buffer)) return end_analize_datagram;
    }

    while(client_datagram.get_next_free_byte() <= recv_length-1) {
        sleep(1);
        if(!client_datagram.get_next_event_length(datagram_buffer, tmp_event_len)) {
            cout<<"next_event_length| recv_len = "<<client_datagram.get_recv_length()<<endl;
            return end_client_program;
        }
        if(!client_datagram.event_checksum_correct(datagram_buffer, tmp_event_len)) {
            cout<<"tmp_event_len "<<tmp_event_len<<endl;
            cout<<"event_checksum_correct\n";
            return end_analize_datagram;
        }

        client_datagram.get_event_no(datagram_buffer, recv_event_no);
        client_datagram.get_next_event_type(datagram_buffer, recv_event_type);

        if(recv_event_type == EVENT_TYPE_NEW_GAME) {
            cout<<"PROBA NEW GAME \n";
            if(!try_to_apply_new_game())
                return end_client_program;
        }
        else {
            if(recv_event_no != next_expected_event_no) {
                cout<<"EVENT NO WRONG\n";
                return end_analize_datagram; //TO DO przejdz do nastepnego eventu gdy event_no juz masz
            }
            if(!try_apply_event())
                return end_client_program;
        }

        cout<<"UZYSKANO EVENT TYPE "<<(int)recv_event_type<<endl;


    }
    return end_analize_datagram;
}


bool Client::try_apply_event() {


    if(recv_event_type == EVENT_TYPE_PIXEL) {
        cout<<"PROBA apply_pixel \n";
        return try_to_apply_pixel();
    }
    if(recv_event_type == EVENT_TYPE_PLAYER_ELIMINATED) {
        cout<<"PROBA player_eliminated \n";
        return try_to_apply_player_eliminated();
    }
    if(recv_event_type == EVENT_TYPE_GAME_OVER) {
        cout<<"PROBA game_over \n";
        return try_to_apply_game_over();
    }
    return next_event;
}


bool Client::try_to_apply_new_game() {
    client_datagram.get_next_event_new_game(datagram_buffer, recv_maxx, recv_maxy, current_game_name_list);
    if(!verify_new_game(recv_maxx, recv_maxy, current_game_name_list)) {
        cout<<"didnt veryfing new game end program\n";
        return end_client_program;
    }

    //start new game
    next_expected_event_no = 1;
    game_continues = true;
    game_over = false;
    not_eliminated_players_number = number_of_players = current_game_name_list.size();
    current_game_id = recv_game_id;
    maxx = recv_maxx;
    maxy = recv_maxy;
    cout<<"maxx i maxy "<<maxx<<" "<<maxy<<endl;

    current_players_names_arr.clear();
    players_not_eliminated.clear();

    for(string name: current_game_name_list){
        current_players_names_arr.push_back(name);
        players_not_eliminated.push_back(true);
    }
    cout<<"UDALO SE NEW GAME \n";
    send_new_game_to_gui();
}


bool Client::try_to_apply_pixel() {
    client_datagram.get_next_event_pixel(datagram_buffer, recv_player_number, recv_x, recv_y);
    if(!verify_pixel_event(recv_player_number, recv_x, recv_y))
        return end_client_program;
    //apply
    next_expected_event_no++;
    //cout<<"send recv_x, recv_y , player_names "<<recv_x<<" "<<recv_y<<" "<<current_players_names_arr[recv_player_number]<<endl;
    send_pixel_to_gui(recv_x, recv_y, current_players_names_arr[recv_player_number]);
}


bool Client::try_to_apply_player_eliminated() {
    client_datagram.get_next_event_player_eliminate(datagram_buffer, recv_player_number);
    if(!verify_player_eliminate(recv_player_number))
        return end_client_program;
    next_expected_event_no++;
    players_not_eliminated[recv_player_number] = false;
    not_eliminated_players_number--;
    send_player_eliminate_to_gui(current_players_names_arr[recv_player_number]);
}


bool Client::try_to_apply_game_over() {
    client_datagram.get_next_event_game_over(datagram_buffer);
    if(!verify_game_over())
        return end_client_program;
    //apply
    next_expected_event_no++;
    game_over = true;
    game_continues = false;
}


bool Client::verify_new_game(const int& maxx, const int& maxy, list<string>& player_names) {
    if(!client_datagram.give_player_list(player_names)) {
        cout<<"give player list wrong data\n";
        return false;
    }
    cout<<"ODEBRALEM LISTE jej rozmiar to "<<player_names.size()<<endl;
    list<string> copy = player_names;
    copy.unique();
    if(player_names.size() == copy.size() && recv_event_type == 0) {
        cout<<"NEW GAME DONE\n";
        return true;
    }
    cout<<"wrong data in new game \n";
    return false;
}

bool Client::verify_pixel_event(const int& player_number, const int& x, const int& y) {
    if(player_number_correct(player_number)) {
        cout<<"PIXEL EVENT DONE\n";
        return x_in_board(x) && y_in_board(y) && players_not_eliminated[player_number];
    }
    cout<<"wrong data in pixel event \n";
    return false;
}

bool Client::verify_player_eliminate(const int& player_number) {
    if(player_number_correct(player_number)) {
        cout<<"PLAYER ELIMINATED APPLED\n";
        return players_not_eliminated[player_number];
    }
    cout<<"wrong data in player eliminated \n";
    return false;
}

bool Client::verify_game_over() {
    if(number_of_players == 1 && game_continues && !game_over) {
        cout<<"GAME OVER DONE\n";
        return true;
    }
    cout<<"wrong data in game over \n";
    return false;
}



