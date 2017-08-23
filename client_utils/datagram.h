#ifndef UNTITLED_DATAGRAM_H
#define UNTITLED_DATAGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <endian.h>
#include <list>


using namespace std;

const int DATAGRAM_SIZE = 512;
const int MIN_CLIENT_DATAGRAM_SIZE = 8 + 1 + 4 + 1;//uint64 + int8 + uint32 + player_name

const int SESSION_ID_POS = 0;
const int TURN_DIRECTION_POS = 8;
const int NEXT_EXP_EVENT_NUMBER_POS = 9;
const int PLAYER_NAME = 13;

const int MAX_PLAYER_NAME_LEN = 64;
const int MIN_VALID_PLAYER_NAME_SYMBOL = 33;
const int MAX_VALID_PLAYER_NAME_SYMBOL = 126;

const int EVENT_TYPE_POS = 0;
const int LEN_POS = 0;
const int EVENT_NO = 4;
const int EVENT_TYPE = 8;
const int EVENT_DATA_POS = 9;
const int CRC32_PIXEL_POS = EVENT_DATA_POS + 9;
const int CRC32_PLAYER_ELIMINATE_POS = EVENT_DATA_POS + 1;
const int CRC32_GAME_OVER_POS = EVENT_DATA_POS;
const int uint64_len = 8;
const int uint32_len = 4;
const int int8_len = 1;

const int LAST_BYTE_OF_NEW_GAME_maxy = 16;
const int LAST_BYTE_OF_PIXEL_EVENT = CRC32_PIXEL_POS + uint64_len-1;
const int LAST_BYTE_OF_PLAYER_ELIMINATED_EVENT = CRC32_PLAYER_ELIMINATE_POS + uint64_len-1;
const int LAST_BYTE_OF_GAME_OVER_EVENT = CRC32_GAME_OVER_POS + uint64_len-1;





class Datagram {

protected:
    //char datagram_buffer[DATAGRAM_SIZE+1];// \0
    char crc32_new_game_buffer[DATAGRAM_SIZE+1];
    char crc32_pixel_buffer[CRC32_PIXEL_POS];
    char crc32_player_eliminate_buffer[CRC32_PLAYER_ELIMINATE_POS];
    char crc32_game_over_buffer[CRC32_GAME_OVER_POS];

    char players_names_buffer[DATAGRAM_SIZE];

    int next_free_byte;
    int recv_length;
    int next_event_start;
    uint32_t event_len;
    uint32_t crc32;



protected:
    void get_int8_bit_value_fbuffer(const char* datagram, int8_t& v, int start_in_datagram);
    void get_int32_bit_value_fbuffer(const char* datagram, uint32_t& v, int start_in_datagram);
    void get_int64_bit_value_fbuffer(const char* datagram, uint64_t& v, int start_in_datagram);
    void get_string_fbuffer(const char* datagram, string& s, int start_in_datagram, int end_in_datagram);

    bool checksum_crc32(const string& recv_fields, uint32_t recv_checksum);
    bool event_within_datagram(const int &end);

    bool give_player_list(const char* datagram, list<string>& player_names);


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

    bool get_game_id(const char* datagram, uint32_t& game_id);
    bool get_next_event_type(const char* datagram, int8_t& event_type);
    //gdy ponizsze funkcje zwracaja true, to znaczy ze wczytano sensowne dane, ale wciaz nalezy je sprawdzic
    bool get_next_event_new_game(const char* datagram, uint32_t& maxx, uint32_t& maxy, list<string>& player_names);
    bool get_next_event_pixel(const char* datagram, int8_t& player_number, uint32_t&x, uint32_t&y);
    bool get_next_event_player_eliminate(const char* datagram, int8_t& player_number);
    bool get_next_event_game_over(const char* datagram);



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
