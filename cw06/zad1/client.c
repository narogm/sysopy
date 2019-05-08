//
// Created by mateusz on 30.04.19.
//

#include <stdio.h>
#include "common.h"

int msgid = -1;
int server_id = -1;

void initialize(){
    /**
    Klient bezpośrednio po uruchomieniu tworzy kolejkę z unikalnym kluczem IPC  i wysyła jej klucz komunikatem do serwera (komunikat INIT).
    Po otrzymaniu takiego komunikatu, serwer otwiera kolejkę klienta, przydziela klientowi identyfikator
    (np. numer w kolejności zgłoszeń) i odsyła ten identyfikator do klienta
    (komunikacja w kierunku serwer->klient odbywa się za pomocą kolejki klienta).
    Po otrzymaniu identyfikatora, klient rozpoczyna wysyłanie zleceń do serwera (w pętli),
    zlecenia są czytane ze standardowego wyjścia w postaci typ_komunikatu albo z pliku tekstowego w którym w każdej linii znajduje się jeden komunikat
    (napisanie po stronie klienta READ plik zamiast typu komunikatu)
*/
    key_t key = ftok("client", getpid());
    msgid = msgget(key, 0666|IPC_CREAT);

    key_t server_key = ftok("server", 0);
    server_id = msgget(server_key, 0);

    struct msg message;
    message.msg_type=INIT;
    sprintf(message.msg_data, "%i", key);

    printf("wyslanie wiadomosci\n");
    msgsnd(server_id, &message, sizeof(message), 0);


}

int main(){
	initialize();
	while(1){
	    ;
	}
}