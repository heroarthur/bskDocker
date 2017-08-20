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


const char LINE_END = 10;
const int MESSAGE_FROM_GUI_LENGTH = 20;


//server configuration variables
string game_server_host, game_server_port = "12345";
int server_sockfd;
struct addrinfo server_ints, *servinfo, *p;


//gui configuration variables
string ui_server_host = "localhost", ui_server_port = "12346";
int gui_sockfd;
struct addrinfo gui_ints, *guiinfo;
string gui_buffer;

//client control
int8_t current_turn_direction = 0;
uint32_t current_game_id = 0;
uint32_t next_expected_event_no = 0;
const uint64_t CLIENT_TO_SERVER_DATAGRAM_INTERVAL = 20000; /* 20ms */
//Clock* global_clock;
list<string> current_players_names; /* cleared with every new game */
int64_t last_sent_event_number = -1; /* number of the last event that was sent to the gui */

char fgui[20];

//moze do wywalenia
int numbytes;


pair <string,string> g = make_pair("as","sad");

//


bool name_is_assiociate_with_port(string name)
{
    for(int i = (int)name.length()-1; i >= 0; i--){
        if('0' <= name[i] && name[i] <= '9'){
            continue;
        }
        if(name[i] == ':'){
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

pair <string,string> break_into_the_address_and_port(string name){
    auto ss = (int)name.length();
    for(int i = (int)name.length()-1; i >= 0; i--){
        if(name[i] == ':'){
            ss = i;
            break;
        }
    }
    pair <string,string> server_port = make_pair(name.substr(0,ss), name.substr(ss+1, name.length()-1));
    if(server_port.first.length() == 0) {
        fprintf(stderr, "server name empty");
    }
    return server_port;
}


int ip_version(const char *src) {
    char buf[16];
    if (inet_pton(AF_INET, src, buf)) {
        return 4;
    } else if (inet_pton(AF_INET6, src, buf)) {
        return 6;
    }
    return -1;
}


void assign_addres_and_port(string& addres, string& port, const string &name_to_break){
    if(name_is_assiociate_with_port(name_to_break)) {
        pair <string,string> addres_port = break_into_the_address_and_port(name_to_break);
        addres = addres_port.first;
        port = addres_port.second;
    }
    else {
        addres = name_to_break;
    }
}


void assign_arguments(int argc, char *argv[]){
    if(argc == 3) {
        assign_addres_and_port(ui_server_host, ui_server_port, string(argv[2]));
    }
    assign_addres_and_port(game_server_host, game_server_port, string(argv[1]));
}


bool connect_to_server() {
    int rv;
    //clear and set struct addrinfo
    memset(&server_ints, 0, sizeof server_ints);
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


bool connect_to_gui() {
    int rv;
    int flag = 1;
    int err;
    //clear and set struct addrinfo
    memset(&gui_ints, 0, sizeof gui_ints);
    gui_ints.ai_family = AF_UNSPEC;
    gui_ints.ai_socktype = SOCK_STREAM;

    //getaddrinfo
    if ((rv = getaddrinfo(ui_server_host.c_str(), ui_server_port.c_str(), &gui_ints, &guiinfo)) != 0) {
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
    return true;
}



void clear_gui_buffer(string& buf) {
    buf = "";
}


void new_game_to_gui_buffer(int maxx, int maxy, list<string> player_names, string& buffer) {
    string v = "NEW_GAME " + to_string(maxx) + " " + to_string(maxy);

    /*for (list<string>::iterator it=player_names.begin(); it != player_names.end(); ++it) {
        v += " " + *it;
    }*/
    for (auto i : player_names){
        v += " " + i;
    }

    v += "\n";
    buffer += buffer + v;
}

void pixel_to_gui_buffer(int x, int y, const string& player_name, string& buffer) {
    string v = "PIXEL " + to_string(x) + " " + to_string(y) + " " + player_name + "\n";
    buffer += buffer + v;
}

void player_eliminated_to_gui_buffer(const string& player_name, string& buffer) {
    string v = "PLAYER_ELIMINATED " + player_name + "\n";
    buffer += buffer + v;
}


int8_t take_direction_from_gui_message(string button_received) {
    string LEFT_BUTTON_DOWN = "LEFT_KEY_DOWN\0";
    string LEFT_BUTTON_UP = "LEFT_KEY_UP\0";
    string RIGHT_BUTTON_DOWN = "RIGHT_KEY_DOWN\0";
    string RIGHT_BUTTON_UP = "RIGHT_KEY_UP\0";

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
    cout<<"WYNOSI "<<button_received<<endl;
    return -2;
}


bool check_if_message_from_gui(int8_t& current_direction) {
    ssize_t rcv_len;
    auto buffer = new char[MESSAGE_FROM_GUI_LENGTH];
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
        if (rcv_len == 0) {
            perror("gui disconnected");
            return false;
        }

    }
    cout<<"DATA "<<data<<endl;
    data = string(data);
    delete[] buffer;
    current_direction = take_direction_from_gui_message(data);
    //cout<<"wartos nowa buttoan to: "<<current_direction<<endl;

    return true;
}



int main (int argc, char *argv[]) {
    int connection_succesful = 1;

    assign_arguments(argc, argv);
    //(int)connect_to_server() *
    connection_succesful = (int)connect_to_gui();

    freeaddrinfo(guiinfo);
    freeaddrinfo(servinfo);

    if(!connection_succesful) {
        return 1;
    }

    send(gui_sockfd, "NEW_GAME 400 600 karol korwin \n", sizeof("NEW_GAME 400 600 karol korwin \n"), 0);

    bool petla = true;
    while(petla){
        //check_if_message_from_gui(current_turn_direction);
    }


    return 0;
}

