#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <endian.h>
#include <iostream>
#include <string>
#include "datagram.h"
#include <boost/crc.hpp>




void Datagram::clear_datagram(char* datagram) {
    memset(datagram, 0, DATAGRAM_SIZE);
    next_free_byte = 0;
    recv_length = 0;
}


void Datagram::pack_next_int8_bit_value_buffer(char* datagram, int8_t v) {
    datagram[next_free_byte] = ((uint8_t*)&v)[0];
    next_free_byte++;
}

void Datagram::pack_next_uint32_bit_value_buffer(char* datagram, uint32_t v) {
    uint32_t net_order = htobe32(v);
    for (int i=0; i<4 ;++i) {
        datagram[next_free_byte] = ((uint8_t*)&net_order)[i];
        next_free_byte++;
    }
}

void Datagram::pack_next_uint64_bit_value_buffer(char* datagram, uint64_t v) {
    uint64_t net_order = htobe64(v);
    for (int i=0; i<8 ;++i) {
        datagram[next_free_byte] = ((uint8_t*)&net_order)[i];
        next_free_byte++;
    }
}


void Datagram::pack_next_string_buffer(char* datagram, string s) {
    //mempcpy(datagram_buffer+next_free_byte, s.c_str(), s.length());
    cout<<s<<endl;
    for(int i = 0; i < s.length(); i++) {
        datagram[next_free_byte] = (char)s[i];
        next_free_byte++;
    }
    cout<<"next "<<next_free_byte<<endl;
}


void Datagram::get_int8_bit_value_fbuffer(const char* datagram, int8_t& v, int start_in_datagram) {
    uint8_t a[1];
    mempcpy(a, datagram + start_in_datagram, sizeof(uint8_t));
    v = *((uint8_t*) a);
}

void Datagram::get_int32_bit_value_fbuffer(const char* datagram, uint32_t& v, int start_in_datagram) {
    uint8_t a[4];
    mempcpy(a, datagram + start_in_datagram, sizeof(uint32_t));
    v = *((uint32_t*) a);
    v = be32toh(v);
}

void Datagram::get_int64_bit_value_fbuffer(const char* datagram, uint64_t& v, int start_in_datagram) {
    uint8_t a[8];
    mempcpy(a, datagram + start_in_datagram, sizeof(uint64_t));
    v = *((uint64_t*) a);
    v = be64toh(v);
}

void Datagram::get_string_fbuffer(const char* datagram, string& s, int start_in_datagram, int end_in_datagram) {
    size_t s_len = size_t (end_in_datagram - start_in_datagram + 1);
    char recv[s_len];
    mempcpy(recv, datagram + start_in_datagram, s_len);
    s = string(recv);
}




void Client_datagram::create_datagram_for_server(char* datagram,
                                                  const uint64_t &session_id, const int8_t &turn_direction,
                                                  const uint32_t &next_expected_event_no, const string& player_name) {
    clear_datagram(datagram);
    pack_next_uint64_bit_value_buffer(datagram, session_id);
    pack_next_int8_bit_value_buffer(datagram, turn_direction);
    pack_next_uint32_bit_value_buffer(datagram, next_expected_event_no);
    pack_next_string_buffer(datagram, player_name);
}




bool Client_datagram::read_datagram_from_client(const char* datagram,
                                                uint64_t& session_id, int8_t& turn_direction,
                                                uint32_t& next_expected_event_no, string& player_name) {
    if(recv_length < MIN_CLIENT_DATAGRAM_SIZE)
        return false;
    get_int64_bit_value_fbuffer(datagram, session_id, SESSION_ID_POS);
    get_int8_bit_value_fbuffer(datagram, turn_direction, TURN_DIRECTION_POS);
    get_int32_bit_value_fbuffer(datagram, next_expected_event_no, NEXT_EXP_EVENT_NUMBER_POS);
    get_string_fbuffer(datagram, player_name, PLAYER_NAME, recv_length-1);

    if(player_name.length() > MAX_PLAYER_NAME_LEN)
        return false;

    for(char c : player_name){
        if(c < MIN_VALID_PLAYER_NAME_SYMBOL ||
           c > MAX_VALID_PLAYER_NAME_SYMBOL) {
            return false;
        }
    }

    return true;
}







bool Server_datagram::read_datagram_from_client(const char* datagram,
                                                uint64_t& session_id, int8_t& turn_direction,
                                                uint32_t& next_expected_event_no, string& player_name) {
    if(recv_length < MIN_CLIENT_DATAGRAM_SIZE)
        return false;
    get_int64_bit_value_fbuffer(datagram, session_id, SESSION_ID_POS);
    get_int8_bit_value_fbuffer(datagram, turn_direction, TURN_DIRECTION_POS);
    get_int32_bit_value_fbuffer(datagram, next_expected_event_no, NEXT_EXP_EVENT_NUMBER_POS);
    get_string_fbuffer(datagram, player_name, PLAYER_NAME, recv_length-1);

    if(player_name.length() > MAX_PLAYER_NAME_LEN)
        return false;

    for(char c : player_name){
        if(c < MIN_VALID_PLAYER_NAME_SYMBOL ||
           c > MAX_VALID_PLAYER_NAME_SYMBOL) {
            return false;
        }
    }

    return true;
}
bool Client_datagram::get_game_id(const char* datagram, uint32_t& game_id) {
    get_int32_bit_value_fbuffer(datagram, game_id, 0);
}


/*
const int LEN_POS = 0;
const int EVENT_NO = 4;
const int EVENT_TYPE = 8;
const int EVENT_DATA_POS = 9;
const int CRC32_PIXEL_POS = EVENT_DATA_POS + 9;
const int CRC32_PLAYER_ELIMINATE_POS = EVENT_DATA_POS + 1;
const int CRC32_GAME_OVER_POS = EVENT_DATA_POS;
 */

bool Client_datagram::get_next_event_type(const char* datagram, int8_t& event_type) {
    get_int8_bit_value_fbuffer(datagram, event_type, next_event_start + LEN_POS);
}

bool Datagram::checksum_crc32(const string& recv_fields, uint32_t recv_checksum) {
    boost::crc_32_type result;
    result.process_bytes(recv_fields.data(), recv_fields.length());
    return result.checksum() == recv_checksum;
}

bool Datagram::event_within_datagram(const int &end) {
    return recv_length >= end;
}


bool Datagram::give_player_list(const char* datagram, list<string>& player_names) {
    memset(players_names_buffer, 0, DATAGRAM_SIZE);
    mempcpy(players_names_buffer, datagram + EVENT_DATA_POS + 2*uint32_len, event_len - int8_len - 3*uint32_len);

    string t = "";
    if(players_names_buffer[event_len - int8_len - 3*uint32_len -1] != '\0')
        return false;
    players_names_buffer[event_len - int8_len - 3*uint32_len -1] = ' ';

    for(int i = 0; i < event_len - int8_len - 3*uint32_len; i++) {
        if(MIN_VALID_PLAYER_NAME_SYMBOL <= players_names_buffer[i]
           && players_names_buffer[i] <= MAX_VALID_PLAYER_NAME_SYMBOL) {
            t += players_names_buffer[i];
        }
        if(players_names_buffer[i] == ' ') {
            player_names.push_back(t);
            t = "";
        }
    }
    return true;
}



bool Client_datagram::get_next_event_new_game(const char *datagram, uint32_t &maxx, uint32_t &maxy,
                                              list<string> &player_names) {

    get_int32_bit_value_fbuffer(datagram, event_len, next_event_start + LEN_POS);
    if(!event_within_datagram(next_event_start + event_len-1))
        return false;

    get_int32_bit_value_fbuffer(datagram, maxx, next_event_start + EVENT_DATA_POS + int8_len);
    get_int32_bit_value_fbuffer(datagram, maxy, next_event_start + EVENT_DATA_POS + int8_len + uint32_len);
    get_int32_bit_value_fbuffer(datagram, crc32, next_event_start + event_len + uint32_len);

    memset(crc32_new_game_buffer, 0, DATAGRAM_SIZE);
    mempcpy(crc32_new_game_buffer, datagram + next_event_start, uint32_len + event_len);

    if(!give_player_list(datagram, player_names))
        return false;

    next_event_start += event_len + uint32_len + uint32_len;
    return checksum_crc32(string(crc32_pixel_buffer), crc32);
}



bool Client_datagram::get_next_event_pixel(const char *datagram, int8_t &player_number, uint32_t &x, uint32_t &y) {
    //sprawdzamy czy dane mieszcza sie w datagramie
    if(!event_within_datagram(next_event_start + LAST_BYTE_OF_PIXEL_EVENT))
        return false;
    mempcpy(crc32_pixel_buffer, datagram + next_event_start, CRC32_PIXEL_POS);
    //wyciagamy dane
    get_int32_bit_value_fbuffer(datagram, event_len, next_event_start + LEN_POS);
    get_int8_bit_value_fbuffer(datagram, player_number, next_event_start + EVENT_DATA_POS);
    get_int32_bit_value_fbuffer(datagram, x, next_event_start + EVENT_DATA_POS + int8_len);
    get_int32_bit_value_fbuffer(datagram, y, next_event_start + EVENT_DATA_POS + int8_len + uint32_len);
    get_int32_bit_value_fbuffer(datagram, crc32, next_event_start + CRC32_PIXEL_POS);
    next_event_start += LAST_BYTE_OF_PIXEL_EVENT + 1;
    //true gdy suma kontrolna sie zgadza
    return checksum_crc32(string(crc32_pixel_buffer), crc32);
}



bool Client_datagram::get_next_event_player_eliminate(const char *datagram, int8_t &player_number) {
    //sprawdzamy czy dane mieszcza sie w datagramie
    if(!event_within_datagram(next_event_start + LAST_BYTE_OF_PLAYER_ELIMINATED_EVENT))
        return false;
    mempcpy(crc32_player_eliminate_buffer, datagram + next_event_start, CRC32_PLAYER_ELIMINATE_POS);
    get_int32_bit_value_fbuffer(datagram, event_len, next_event_start + LEN_POS);
    //wyciagamy dane
    get_int32_bit_value_fbuffer(datagram, crc32, next_event_start + CRC32_PLAYER_ELIMINATE_POS);
    next_event_start += LAST_BYTE_OF_PLAYER_ELIMINATED_EVENT + 1;
    //true gdy suma kontrolna sie zgadza
    return checksum_crc32(string(crc32_player_eliminate_buffer), crc32);
}



bool Client_datagram::get_next_event_game_over(const char *datagram) {
    //sprawdzamy czy dane mieszcza sie w datagramie
    if(!event_within_datagram(next_event_start + LAST_BYTE_OF_GAME_OVER_EVENT))
        return false;
    mempcpy(crc32_game_over_buffer, datagram + next_event_start, CRC32_GAME_OVER_POS);
    get_int32_bit_value_fbuffer(datagram, event_len, next_event_start + LEN_POS);
    next_event_start += LAST_BYTE_OF_GAME_OVER_EVENT + 1;
    //true gdy suma kontrolna sie zgadza
    return checksum_crc32(string(crc32_player_eliminate_buffer), crc32);
}









