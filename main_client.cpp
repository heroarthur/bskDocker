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
#include "client_utils/client/client_utils.h"
#include "client_utils/client/client.h"
#include "client_utils/datagram/datagram.h"
#include<time.h>

using namespace std;


string game_server_host = "localhost", game_server_port = "12345";
string game_ui_host = "localhost", game_ui_port = "12346";



int main (int argc, char *argv[]) {
    int connection_succesful = 1;
    /*assign_program_parameters(argc, argv ,
                              game_ui_host, game_ui_port,
                              game_server_host, game_server_port);
    */

    int8_t l_8 = -1;
    uint32_t l_32 = 12345;
    uint64_t l_64 = 987654321;
    string player_name = "MORAWIECKI_KRZYSZTOF"; //MORAWIECKI_KRZYSZTOF

    Client client = Client("imie",
                           game_server_host, game_server_port,
                           game_ui_host, game_ui_port); //TO DO popraw parsowanie, aby parsowal player_name


    int8_t turn_dir = -1;
    uint32_t next_event_number = 12345;
    uint64_t session_id = 987654321;
    string player_name_recv;

    Client_datagram datagram = Client_datagram();

    datagram.create_datagram_for_server(client.datagram_buffer, l_64, l_8, l_32, player_name);
    datagram.read_datagram_from_client(client.datagram_buffer, session_id, turn_dir, next_event_number, player_name_recv);

    cout<<"session_id "<<session_id<<endl;
    cout<<"turn_dir "<<(int)turn_dir<<endl;
    cout<<"next_event_number "<<next_event_number<<endl;
    cout<<"player_name_recv "<<player_name_recv<<endl;




    if(!client.connect_to_gui())
        return 1;



    bool t = true;

    while(t) {
        if(client.should_sent_next_datagram()) {
            //if(!client.send_datagram_to_server()) return 1;
        }
        else {
            //receive tcp
            client.update_direction();
            //receive udp

        }

    }

    return 0;
}

