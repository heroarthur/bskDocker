#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>
#include <iostream>


using namespace std;


bool name_is_assiociate_with_port(string name)
{
    for(int i = (int)name.length()-1; i >= 0; i--){
        if('0' <= name[i] && name[i] <= '9'){
            continue;
        }
        if(name[i] == ':'){
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

pair <string,string> break_into_the_address_and_port(string name){
    auto ss = (int)name.length();
    for(int i = (int)name.length()-1; i >= 0; i--){
        if(name[i] == ':'){
            ss = i;
            break;
        }
    }
    pair <string,string> server_port = make_pair(name.substr(0,ss), name.substr(ss+1, name.length()-1));
    if(server_port.first.length() == 0) {
        fprintf(stderr, "server name empty");
    }
    return server_port;
}



void assign_addres_and_port(string& addres, string& port, const string &name_to_break){
    if(name_is_assiociate_with_port(name_to_break)) {
        pair <string,string> addres_port = break_into_the_address_and_port(name_to_break);
        addres = addres_port.first;
        port = addres_port.second;
    }
    else {
        addres = name_to_break;
    }
}


bool assign_program_parameters(int argc, char **argv,
                               string& game_ui_host, string& game_ui_port,
                               string& game_server_host, string& game_server_port){
    if(argc == 1) {
        cout<<"not specified server address\n";
        return false;
    }

    if(argc == 3) {
        assign_addres_and_port(game_ui_host, game_ui_port, string(argv[2]));
    }
    assign_addres_and_port(game_server_host, game_server_port, string(argv[1]));
    return true;
}