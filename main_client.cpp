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


    /*
    bool pack_pixel_to_datagram(char* client_datagram, const uint32_t& event_no, const int8_t& player_number, const uint32_t&x, const uint32_t&y);
    bool pack_player_eliminate(char* client_datagram, const uint32_t& event_no, const int8_t& player_number);
    bool pack_game_over_to_datagram(char* client_datagram, const uint32_t& event_no);*/
    char datagram[DATAGRAM_SIZE+1];

    uint32_t next_event_no_pack = 1;
    int8_t player_number_pack = 2;
    int8_t turn_dir_pack;
    uint32_t x = 5, y = 6;

    uint64_t l_64 = 987654321;
    string player_name = "MORAWIECKI_KRZYSZTOF"; //MORAWIECKI_KRZYSZTOF

    uint32_t next_event_no_recv;
    int8_t player_number_recv;
    int8_t turn_dir_recv;
    uint32_t x_recv, y_recv;

/*
    Client client = Client("imie",
                           game_server_host, game_server_port,
                           game_ui_host, game_ui_port); //TO DO popraw parsowanie, aby parsowal player_name
*/

    Client_datagram dat = Client_datagram();
    Server_datagram serv = Server_datagram();

    memset(datagram, 0, DATAGRAM_SIZE+1);
    list<string> l;
    l.push_back("1234567890");
    l.push_back("12345678901234567890");
    l.push_back("123456789012345678901234567890");

    list<string> recv_l;

    serv.pack_new_game_to_datagram(datagram, next_event_no_pack, x, y, l);
    serv.pack_pixel_to_datagram(datagram, next_event_no_pack, player_number_pack, x, y);

    cout<<"next free_byte "<<serv.get_next_free_byte()<<endl;

    cout<<dat.get_next_event_new_game(datagram, x, y, recv_l)<<endl;
    cout<<"kolejne dane\n";
    cout<<"x_recv "<<x_recv<<endl;
    cout<<"y_recv "<<y_recv<<endl;
    for(string k: recv_l) {
        cout<<k<<endl;
    }
    cout<<endl;

    cout<<dat.get_next_event_pixel(datagram, player_number_recv, x_recv, y_recv)<<endl;

    cout<<"kolejne dane\n";
    cout<<"p_number "<<(int)player_number_recv<<endl;
    cout<<"x_recv "<<x_recv<<endl;
    cout<<"y_recv "<<y_recv<<endl<<endl;

    /*
    for(int i = 0; i < 1; i++) {
        next_event_no_pack = i*10;
        player_number_pack = i;
        x = i*100;
        y = i*100;
        if(!serv.pack_pixel_to_datagram(client_datagram, next_event_no_pack, player_number_pack, x, y)){
            cout<<"KONIEC MIEJSCA SERVER i = "<<i<<endl;
            break;
        }
        else {

            //cout<<"d";
        }
    }
    cout<<"next_free_byte "<<serv.get_next_free_byte();
    */

/*
    for(int i = 0; i < 100; i++) {
        if(!dat.get_next_event_pixel(client_datagram, player_number_recv, x_recv, y_recv)){
            cout<<"KONIEC MIEJSCA KLIENT i = "<<i<<endl;
            break;
        }
        cout<<"kolejne dane\n";
        cout<<"p_number "<<player_number_recv<<endl;
        cout<<"x_recv "<<x_recv<<endl;
        cout<<"y_recv "<<y_recv<<endl<<endl;

    }
*/


    //bool get_next_event_pixel(const char* client_datagram, int8_t& player_number, uint32_t&x, uint32_t&y);
    //bool get_next_event_player_eliminate(const char* client_datagram, int8_t& player_number);
    //bool get_next_event_game_over(const char* client_datagram);


    //client_datagram.create_datagram_for_server(client.datagram_buffer, l_64, l_8, l_32, player_name);
    //client_datagram.read_datagram_from_client(client.datagram_buffer, session_id, turn_dir, next_event_number, player_name_recv);

    //cout<<"session_id "<<session_id<<endl;
    //cout<<"turn_dir "<<(int)turn_dir<<endl;
    //cout<<"next_event_number "<<next_event_number<<endl;
    //cout<<"player_name_recv "<<player_name_recv<<endl;



/*
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
*/
    return 0;
}

