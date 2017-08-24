#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <endian.h>
#include <iostream>
#include <string>
#include "datagram.h"
#include <boost/crc.hpp>





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


bool Server_datagram::data_will_fit_to_datagram(const int &last_byte_pos) {
    return DATAGRAM_SIZE-1 >= last_byte_pos;
}


int Server_datagram::length_of_new_game_event(const string& player_names) {
    auto length_of_names = (int)player_names.length();
    //length_of_names += 1; // '\0' after last name
    return SIZE_BYTE_OF_NEW_GAME_to_maxy + length_of_names;
}


bool Server_datagram::pack_new_game_to_datagram(char* datagram, const uint32_t& event_no, const uint32_t& maxx, const uint32_t& maxy,
                                                const list<string>& player_names) {

    string name_list = chain_list(player_names);
    int new_game_event_len = length_of_new_game_event(name_list);
    if(!data_will_fit_to_datagram(next_free_byte + new_game_event_len + uint32_len -1))
        return false;
    pack_next_uint32_bit_value_buffer(datagram, new_game_event_len - uint32_len);
    pack_next_uint32_bit_value_buffer(datagram, event_no);
    pack_next_int8_bit_value_buffer(datagram, EVENT_TYPE_NEW_GAME);
    pack_next_uint32_bit_value_buffer(datagram, maxx);
    pack_next_uint32_bit_value_buffer(datagram, maxy);
    pack_name_list_buffer(datagram, player_names);
    pack_next_uint32_bit_value_buffer(datagram, checksum_current_new_game(datagram, new_game_event_len));
    return true;
}

bool Server_datagram::pack_pixel_to_datagram(char* datagram, const uint32_t& event_no, const int8_t& player_number,
                                             const uint32_t&x, const uint32_t&y) {
    if(!data_will_fit_to_datagram(next_free_byte + SIZE_BYTE_OF_PIXEL_EVENT -1))
        return false;

    pack_next_uint32_bit_value_buffer(datagram, PIXEL_EVENTS_FIELDS_SIZE);
    pack_next_uint32_bit_value_buffer(datagram, event_no);
    pack_next_int8_bit_value_buffer(datagram, EVENT_TYPE_PIXEL);
    pack_next_int8_bit_value_buffer(datagram, player_number);
    pack_next_uint32_bit_value_buffer(datagram, x);
    pack_next_uint32_bit_value_buffer(datagram, y);
    pack_next_uint32_bit_value_buffer(datagram, checksum_current_pixel(datagram));
    return true;
}

bool Server_datagram::pack_player_eliminate(char* datagram, const uint32_t& event_no,
                                            const int8_t& player_number) {
    if(!data_will_fit_to_datagram(current_event_start + SIZE_BYTE_OF_PLAYER_ELIMINATE_EVENT -1))
        return false;
    pack_next_uint32_bit_value_buffer(datagram, PLAYER_ELIMINATED_EVENTS_FIELDS_SIZE);
    pack_next_uint32_bit_value_buffer(datagram, event_no);
    pack_next_int8_bit_value_buffer(datagram, EVENT_TYPE_PLAYER_ELIMINATED);
    pack_next_int8_bit_value_buffer(datagram, player_number);
    pack_next_uint32_bit_value_buffer(datagram, checksum_current_player_eliminated(datagram));
    return true;
}


bool Server_datagram::pack_game_over_to_datagram(char* datagram, const uint32_t& event_no) {

    if(!data_will_fit_to_datagram(current_event_start + SIZE_BYTE_OF_GAME_OVER_EVENT -1))
        return false;
    pack_next_uint32_bit_value_buffer(datagram, PLAYER_ELIMINATED_EVENTS_FIELDS_SIZE);
    pack_next_uint32_bit_value_buffer(datagram, event_no);
    pack_next_int8_bit_value_buffer(datagram, EVENT_TYPE_PLAYER_ELIMINATED);
    pack_next_uint32_bit_value_buffer(datagram, checksum_current_game_over(datagram));
    return true;
}
