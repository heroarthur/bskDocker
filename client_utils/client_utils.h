#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <utility>


#ifndef UNTITLED_CLIENT_UTILS_H
#define UNTITLED_CLIENT_UTILS_H

using namespace std;

bool name_is_assiociate_with_port(string name);

pair <string,string> break_into_the_address_and_port(string name);

void assign_addres_and_port(string& addres, string& port, const string &name_to_break);

bool assign_program_parameters(int argc, char **argv,
                               string& game_ui_host, string& game_ui_port,
                               string& game_server_host, string& game_server_port);

#endif //UNTITLED_CLIENT_UTILS_H
