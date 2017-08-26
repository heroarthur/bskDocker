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



int main_ (int argc, char *argv[]) {
    int connection_succesful = 1;
    /*assign_program_parameters(argc, argv ,
                              game_ui_host, game_ui_port,
                              game_server_host, game_server_port);
    */


    /*
    bool pack_pixel_to_datagram(char* client_datagram, const uint32_t& event_no, const int8_t& player_number, const uint32_t&x, const uint32_t&y);
    bool pack_player_eliminate(char* client_datagram, const uint32_t& event_no, const int8_t& player_number);
    bool pack_game_over_to_datagram(char* client_datagram, const uint32_t& event_no);*/
    char datagram[DATAGRAM_SIZE+1];
    memset(datagram, 0, DATAGRAM_SIZE+1);

    uint32_t next_event_no_pack = 0;
    int8_t player_number_pack = 2;
    int8_t turn_dir_pack;
    uint32_t x = 400, y = 600;
    uint64_t l_64 = 987654321;
    string player_name = "MORAWIECKI_KRZYSZTOF";
    list<string> l; l.push_back("karol"); l.push_back("korwin"); l.push_back("max");

    Client client = Client("imie",
                           game_server_host, game_server_port,
                           game_ui_host, game_ui_port); //TO DO popraw parsowanie, aby parsowal player_name
    Client_datagram dat = Client_datagram();
    Server_datagram serv = Server_datagram();

    serv.pack_game_id(datagram, 5);
    serv.pack_new_game_to_datagram(datagram, next_event_no_pack, x, y, l);
    serv.pack_pixel_to_datagram(datagram, 1, 2, 10, 60);
    serv.pack_pixel_to_datagram(datagram, 2, 2, 9, 60);


    serv.pack_player_eliminate(datagram, 3, 2);
    serv.pack_game_over_to_datagram(datagram, 4);


    uint32_t recv_id;
    dat.get_game_id(datagram, recv_id);

    //bool get_next_event_pixel(const char* client_datagram, int8_t& player_number, uint32_t&x, uint32_t&y);
    //bool get_next_event_player_eliminate(const char* client_datagram, int8_t& player_number);
    //bool get_next_event_game_over(const char* client_datagram);



    if(!client.connect_to_gui())
        return 1;

    sleep(4);
    cout<<"ruszam, ilosc bajtow w datagramie: "<<serv.get_next_free_byte()<<endl;
    client.receive_datagram_from_server_test(datagram, serv.get_next_free_byte());
    //client.send_new_game_to_gui();


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

