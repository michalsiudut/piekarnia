#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "definicje.h"


void odbieraj_wiadomosc(int msgid) {
    Wypieki wypieki;

    while (1) {
        if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0, 0) == -1) {
            perror("Błąd przy odbieraniu wiadomości");
            break;
        }
        printf("Odebrano produkt: %s | Ilość: %d | Cena: %d\n", wypieki.nazwa,wypieki.liczba_sztuk,wypieki.cena);
    }
}

int main() {
    key_t key = KEY_MSG;
    int msgid;
    // uzyskiwanie dostepu do kolejki
    msgid = msgget(key, 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów");
        exit(1);
    }

    printf("Odbiorca: Rozpoczynam odbiór wypieków.\n");

    odbieraj_wiadomosc(msgid);

    return 0;
}
