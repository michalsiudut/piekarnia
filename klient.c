#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "definicje.h"

void V(int semid, int i, int x) {
    struct sembuf op;
    op.sem_num = x; // Semafor nr x
    op.sem_op = i; // Zwiększ wartość semafora i 
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1) {
        perror("Błąd przy operacji V()");
        exit(1);
    }
}

void odbieraj_wiadomosc(int msgid, int semid) {
    Wypieki wypieki;

    while (1) {
        // branie wypieku z tasmy
        if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0, 0) == -1) {
            perror("Błąd przy odbieraniu wiadomości");
            break;
        }

        printf("-----Odebrano produkt: %s | Ilość: %d | Cena: %d\n", wypieki.nazwa,wypieki.liczba_sztuk,wypieki.cena);
        V(semid,wypieki.liczba_sztuk,wypieki.mtype);
    }
}

int main() {
    key_t key = KEY_MSG;
    int msgid, semid;

    // Uzyskanie dostępu do semafora
    semid = semget(KEY_SEM, 1, 0);
    if (semid == -1) {
        perror("Błąd przy uzyskiwaniu dostępu do semafora");
        exit(1);
    }

    // uzyskiwanie dostepu do kolejki
    msgid = msgget(key, 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów");
        exit(1);
    }

    printf("-----Klient: Zaczynam zakupy\n");

    odbieraj_wiadomosc(msgid, semid);

    return 0;
}