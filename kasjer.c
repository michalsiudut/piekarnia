#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include "definicje.h"


void sem_op(int sem_id, int op) {
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = op;
    sops.sem_flg = 0;
    if (semop(sem_id, &sops, 1) == -1) {
        perror("Błąd operacji na semaforze");
        exit(1);
    }
}

void handle_sigterm(int sig) {
    printf("Kasjer: Zamykam kasę.\n"); //  tu bedzie sczyscienie semaforw i wogole


    // uzyskanie dostepu do kolejki
    int msgid = msgget(KEY_MSG_KLIENT_KASJER, 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki");
        exit(1);
    }

    // usuwanie kolejki
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Błąd przy usuwaniu kolejki");
    } else {
        printf("Kolejka komunikatów została usunięta.\n");
    }
    exit(0);
}

int main() {
    signal(SIGTERM, handle_sigterm);

    int sem_id = semget(SEM_KEY_DO_SKLEPU, 1, 0);
    if (sem_id == -1) {
        perror("Błąd uzyskiwania dostępu do semafora");
        exit(1);
    }

    int msgid = msgget(KEY_MSG_KLIENT_KASJER, 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów@@@@");
        exit(1);
    }

    Wypieki wypieki;  // Struktura do odbioru wiadomości


    while (1) {
        printf("\t\tKasjer: Czekam na wiadomość...\n");
        // Odbieranie wiadomości z kolejki
        if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0, 0) == -1) {
            perror("Błąd msgrcv");
            break;
        }       
        printf("\t\tKasjer: Otrzymałem wiadomość od klienta %d\n", wypieki.mtype);

        printf("\t\tWypiek: %s | Ilość: %d | Cena: %d\n", wypieki.nazwa, wypieki.liczba_sztuk, wypieki.cena);

        // Przetwarzanie zakończone
        int sem_value = semctl(sem_id, 0, GETVAL);
        printf("\t\tAktualna wartość semafora: %d\n", sem_value);
        sem_op(sem_id, 1); 

        usleep(300000);;  
    }
    return 0;
    
}
