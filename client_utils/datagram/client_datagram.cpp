#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <endian.h>
#include <iostream>
#include <string>
#include "datagram.h"
#include <boost/crc.hpp>



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


bool Client_datagram::get_game_id(const char* datagram, uint32_t& game_id) {
    get_int32_bit_value_fbuffer(datagram, game_id, 0);
}


bool Client_datagram::get_next_event_type(const char* datagram, int8_t& event_type) {
    get_int8_bit_value_fbuffer(datagram, event_type, current_event_start + LEN_POS);
}


bool Client_datagram::get_next_event_new_game(const char *datagram, uint32_t &maxx, uint32_t &maxy,
                                              list<string> &player_names) {

    get_int32_bit_value_fbuffer(datagram, event_len, current_event_start + LEN_POS);
    if(!event_within_datagram(current_event_start + event_len-1))
        return false;

    get_int32_bit_value_fbuffer(datagram, maxx, current_event_start + EVENT_DATA_POS + int8_len);
    get_int32_bit_value_fbuffer(datagram, maxy, current_event_start + EVENT_DATA_POS + int8_len + uint32_len);
    get_int32_bit_value_fbuffer(datagram, crc32, current_event_start + event_len + uint32_len);
    if(!give_player_list(datagram, player_names))
        return false;
    current_event_start += event_len + uint32_len + uint32_len;
    return crc32 == checksum_current_new_game(datagram, event_len);
}

bool Client_datagram::get_next_event_pixel(const char *datagram, int8_t &player_number, uint32_t &x, uint32_t &y) {
    //sprawdzamy czy dane mieszcza sie w datagramie
    if(!event_within_datagram(current_event_start + LAST_BYTE_OF_PIXEL_EVENT))
        return false;
    //wyciagamy dane
    get_int32_bit_value_fbuffer(datagram, event_len, current_event_start + LEN_POS);
    get_int8_bit_value_fbuffer(datagram, player_number, current_event_start + EVENT_DATA_POS);
    get_int32_bit_value_fbuffer(datagram, x, current_event_start + EVENT_DATA_POS + int8_len);
    get_int32_bit_value_fbuffer(datagram, y, current_event_start + EVENT_DATA_POS + int8_len + uint32_len);
    get_int32_bit_value_fbuffer(datagram, crc32, current_event_start + CRC32_PIXEL_POS);
    current_event_start += SIZE_BYTE_OF_PIXEL_EVENT;
    //true gdy suma kontrolna sie zgadza
    return crc32 == checksum_current_pixel(datagram);
}


bool Client_datagram::get_next_event_player_eliminate(const char *datagram, int8_t &player_number) {
    //sprawdzamy czy dane mieszcza sie w datagramie
    if(!event_within_datagram(current_event_start + LAST_BYTE_OF_PLAYER_ELIMINATED_EVENT))
        return false;
    get_int32_bit_value_fbuffer(datagram, event_len, current_event_start + LEN_POS);
    //wyciagamy dane
    get_int32_bit_value_fbuffer(datagram, crc32, current_event_start + CRC32_PLAYER_ELIMINATE_POS);
    current_event_start += SIZE_BYTE_OF_PLAYER_ELIMINATE_EVENT;
    //true gdy suma kontrolna sie zgadza
    return crc32 == checksum_current_player_eliminated(datagram);
}


bool Client_datagram::get_next_event_game_over(const char *datagram) {
    //sprawdzamy czy dane mieszcza sie w datagramie
    if(!event_within_datagram(current_event_start + LAST_BYTE_OF_GAME_OVER_EVENT))
        return false;
    mempcpy(crc32_game_over_buffer, datagram + current_event_start, CRC32_GAME_OVER_POS);
    get_int32_bit_value_fbuffer(datagram, event_len, current_event_start + LEN_POS);
    current_event_start += LAST_BYTE_OF_GAME_OVER_EVENT + 1;
    //true gdy suma kontrolna sie zgadza
    return crc32 == checksum_current_game_over(datagram);
}


