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


int main (int argc, char *argv[]) {

    /*
     * while(true) {
     *     if(gra_trwa) {
     *          -wrzuc graczy nie odpowiadajacych do obserwatorow
     *          if(nowa_runda) {
     *              -przesun wszystkich graczy
     *              -wygeneruj zdarzenia
     *              -rozeslij zdarzenia
     *          }
     *     }
     *     else {
     *          -czy zaczac nowa gre?
     *     }
     *     -podlacz nowych graczy i obserwatorow
     *     -przyjmij i odpowiedz na datagramy
     * }
     *
     * */

    /*
     *
     *
     * while(true) {
     *     server.disconnect_not_responding_clients();
     *     if(server.currently_game_ongoing) {
     *          if(server.next_round_start) {
     *              server.execute_next_round();
     *          }
     *     }
     *     else {
     *          server.start_new_game?
     *     }
     *     server.receive_from_clients
     *
     *
     *     dodatkowo:
     *     server.send_datagrams_to_client(client) //wysyla od odebranego next_expected_event_no
     *     server.send_to_all_clients
     * }
     *
     * */

    /*
     * 1 wektor 2 listy
     * -gracze_w_danej_grze -zamrazany na czas gry
     * -gracze oczekujacy -wywalaj odlaczajac
     * -obserwatorzy - puste nazwy,
     *
     *
     *
     *
     *
     *
     *
     * */


    return 0;
}

