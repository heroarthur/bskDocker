#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <regex>
#include <iostream>
#include <fcntl.h>
#include <list>
using namespace std;

/*
void clear_gui_buffer(string& buf) {
    buf = "";
}


void new_game_to_gui_buffer(int maxx, int maxy, list<string> player_names, string& buffer) {
    string v = "NEW_GAME " + to_string(maxx) + " " + to_string(maxy);

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
*/