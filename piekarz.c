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

Wypieki losuj_wypiek();

void init_semaphore(int semid) {
    for(int i = 0; i < 16; i++){
    if (semctl(semid, i, SETVAL, 5) == -1) {
        perror("Błąd przy inicjalizacji semafora");
        exit(1);
    }
    }
}
void P(int semid, int i, int x) {
    struct sembuf op;
    op.sem_num = x; // Semafor nr 0
    op.sem_op = -i; // Zmniejsz wartość semafora
    op.sem_flg = 0;
    //printf("numer semafora: %d , pomniejszona wartosc smeafora: %d\n", x, i);
    if (semop(semid, &op, 1) == -1) {
        perror("Błąd przy operacji P()");
        exit(1);
    }
}

void usuniecie_kolejki(int sig) {
    // uzyskanie dostepu do semaforow
    int semid = semget(KEY_SEM, 1, 0666);
    // uzyskanie dostepu do kolejki
    int msgid = msgget(KEY_MSG, 0666);
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
    // usuwanie semaforow
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Błąd przy usuwaniu semafora");
        exit(1);
    }

    exit(0);
}



int losuj_liczbe(int min, int max) {
    return rand() % max + min;
}

int main() {
    int msgid, semid;
    Wypieki wypieki;

    // inicjalizacja generatora liczb losowych
    srand(time(NULL));
    signal(SIGINT, usuniecie_kolejki);
    // uzyskanie dostepu do kolejki
    msgid = msgget(KEY_MSG, IPC_CREAT | IPC_EXCL | 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów");
        exit(1);
    }


    semid = semget(KEY_SEM, 16, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Błąd przy tworzeniu semafora");
        exit(1);
    }
    init_semaphore(semid);

    printf("Piekarz: Rozpoczynam produkcję wypieków.\n");

    while (1) {
        wypieki = losuj_wypiek();
        P(semid, wypieki.liczba_sztuk, wypieki.mtype);
        // wysylanie losowego wypieku od kolejki
        if (msgsnd(msgid, &wypieki, sizeof(wypieki)- sizeof(long), 0) < 0) {
            perror("Błąd przy wysyłaniu wiadomości");
            usuniecie_kolejki(SIGINT);
            break;
        }

        printf("Piekarz: Wyprodukowano produkt %s w ilości %d w cenie %d.\n",
               wypieki.nazwa, wypieki.liczba_sztuk, wypieki.cena);

        sleep(CZAS_WYPIEKU); // 2 sekund na wypiek
    }

    return 0;
}

// funkcja zwraca losowy wypiek
Wypieki losuj_wypiek() {
    Wypieki wypiek;
    long wybor = losuj_liczbe(1,15); // losowanie

    switch(wybor) {
        case 0:
            strcpy(wypiek.nazwa, "Chleb pszenny");
            wypiek.cena = 3;
            break;
        case 1:
            strcpy(wypiek.nazwa, "Chleb żytni");
            wypiek.cena = 4;
            break;
        case 2:
            strcpy(wypiek.nazwa, "Bułki kajzerki");
            wypiek.cena = 1;
            break;
        case 3:
            strcpy(wypiek.nazwa, "Bułki grahamki");
            wypiek.cena = 5;
            break;
        case 4:
            strcpy(wypiek.nazwa, "Bułki maślane");
            wypiek.cena = 6;
            break;
        case 5:
            strcpy(wypiek.nazwa, "Rogaliki");
            wypiek.cena = 7;
            break;
        case 6:
            strcpy(wypiek.nazwa, "Croissanty");
            wypiek.cena = 8;
            break;
        case 7:
            strcpy(wypiek.nazwa, "Chleb razowy");
            wypiek.cena = 9;
            break;
        case 8:
            strcpy(wypiek.nazwa, "Chleb tostowy");
            wypiek.cena = 10;
            break;
        case 9:
            strcpy(wypiek.nazwa, "Pita");
            wypiek.cena = 11;
            break;
        case 10:
            strcpy(wypiek.nazwa, "Bagietki");
            wypiek.cena = 12;
            break;
        case 11:
            strcpy(wypiek.nazwa, "Ciasto francuskie");
            wypiek.cena = 13;
            break;
        case 12:
            strcpy(wypiek.nazwa, "Pączki");
            wypiek.cena = 14;
            break;
        case 13:
            strcpy(wypiek.nazwa, "Muffiny");
            wypiek.cena = 15;
            break;
        case 14:
            strcpy(wypiek.nazwa, "Wuzetki");
            wypiek.cena = 16;
            break;
    }
    wypiek.mtype = wybor;
    wypiek.liczba_sztuk = losuj_liczbe(1,4); // od 1 do 4 sztuk
    wypiek.cena = wypiek.cena * wypiek.liczba_sztuk;

    return wypiek;
}