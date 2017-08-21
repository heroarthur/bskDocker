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
#include "client_utils/client_utils.h"
#include "client_utils/client.h"

using namespace std;

string game_server_host = "localhost", game_server_port = "12345";
string game_ui_host = "localhost", game_ui_port = "12346";

bool test_getaddrinfo_main() {
    struct addrinfo *p;
    int gui_sockfd;
    struct addrinfo gui_ints, *guiinfo;
    int rv;
    int flag = 1;
    int err;

    memset(&gui_ints, 0, sizeof gui_ints);
    //clear and set struct addrinfo
    gui_ints.ai_family = AF_UNSPEC;
    gui_ints.ai_socktype = SOCK_STREAM;

    //getaddrinfo
    cout<<"wartosci: "<<game_ui_host<<" "<<game_ui_port<<endl;
    if ((rv = getaddrinfo(game_ui_host.c_str(), game_ui_port.c_str(), &gui_ints, &guiinfo)) != 0) {
        fprintf(stderr, "gui getaddrinfo: %s\n", gai_strerror(rv));
        cout<<"no jhjh\n";
        return false;
    }
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




int main (int argc, char *argv[]) {
    int connection_succesful = 1;
    /*assign_program_parameters(argc, argv ,
                              game_ui_host, game_ui_port,
                              game_server_host, game_server_port);
    */
    game_server_host = "localhost"; game_server_port = "12345";
    game_ui_host = "localhost"; game_ui_port = "12346";


    Client client = Client("imie",
                           game_server_host, game_server_port,
                           game_ui_host, game_ui_port); //TO DO popraw parsowanie, aby parsowal player_name

    if(!client.connect_to_gui())
        return 1;

    bool t = true;

    while(t) {
        if(client.should_sent_next_datagram()) {
            //wyslij datagram
        }
        else {
            //receive tcp
            client.update_direction();
            //receive udp

        }

    }
    return 0;
}

