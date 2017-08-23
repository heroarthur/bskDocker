#ifndef UNTITLED_DATAGRAM_H
#define UNTITLED_DATAGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <endian.h>


using namespace std;

const int DATAGRAM_SIZE = 512;
const int MIN_CLIENT_DATAGRAM_SIZE = 8 + 1 + 4 + 1;//uint64 + int8 + uint32 + player_name

const int SESSION_ID_POS = 0;
const int TURN_DIRECTION_POS = 8;
const int NEXT_EXP_EVENT_NUMBER_POS = 9;
const int PLAYER_NAME_POS = 13;

const int MAX_PLAYER_NAME_LEN = 64;
const int MIN_VALID_PLAYER_NAME_SYMBOL = 33;
const int MAX_VALID_PLAYER_NAME_SYMBOL = 126;

class Datagram {

protected:
    char datagram_buffer[DATAGRAM_SIZE+1];// \0
    int next_free_byte;
    int recv_length;


protected:
    void get_int8_bit_value_fbuffer(const char* datagram, int8_t& v, int start_in_datagram);
    void get_int32_bit_value_fbuffer(const char* datagram, uint32_t& v, int start_in_datagram);
    void get_int64_bit_value_fbuffer(const char* datagram, uint64_t& v, int start_in_datagram);
    void get_string_fbuffer(const char* datagram, string& s, int start_in_datagram, int end_in_datagram);

public:
    void clear_datagram(char* datagram);

    void pack_next_int8_bit_value_buffer(char* datagram, int8_t v);
    void pack_next_uint32_bit_value_buffer(char* datagram, uint32_t v);
    void pack_next_uint64_bit_value_buffer(char* datagram, uint64_t v);
    void pack_next_string_buffer(char* datagram, string s);

};




class Client_datagram: public Datagram {
public:
    //klient
    //-odbiera datagram
    //-tworzy wlasny datagram
    void create_datagram_for_server(char* datagram,
                                     const uint64_t &session_id, const int8_t &turn_direction,
                                     const uint32_t &next_expected_event_no, const string& player_name);

    bool read_datagram_from_client(const char* datagram,
                                   uint64_t& session_id, int8_t& turn_direction,
                                   uint32_t& next_expected_event_no, string& player_name);
    //void receive_datagram();
private:
    //-analizuje datagram odebrany
    //wysyla do gui
    //void apply_next_event();
    //void send_to_gui();

};



class Server_datagram: public Datagram {
public:
    bool read_datagram_from_client(const char* datagram,
                                   uint64_t& session_id, int8_t& turn_direction,
                                   uint32_t& next_expected_event_no, string& player_name);

};


#endif //UNTITLED_DATAGRAM_H
