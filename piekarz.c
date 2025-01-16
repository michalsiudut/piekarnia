#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "definicje.h" 

Wypieki losuj_wypiek();


void usuniecie_kolejki(int sig) {
    // Uzyskanie dostępu do kolejki komunikatów
    int msgid = msgget(KEY_MSG, 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki");
        exit(1);
    }

    // Usuwanie kolejki komunikatów
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Błąd przy usuwaniu kolejki");
    } else {
        printf("Kolejka komunikatów została usunięta.\n");
    }

    exit(0); // Zakończenie programu
}



int losuj_liczbe(int min, int max) {
    return rand() % max + min;  
}

int main() {
    int msgid;
    Wypieki wypieki;

    // Inicjalizacja generatora liczb losowych
    srand(time(NULL));
    signal(SIGINT, usuniecie_kolejki);
    // Uzyskanie dostępu do kolejki komunikatów
    msgid = msgget(KEY_MSG, IPC_CREAT | IPC_EXCL | 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów");
        exit(1);
    }

    printf("Piekarz: Rozpoczynam produkcję wypieków.\n");

    while (1) {
        wypieki = losuj_wypiek();
        // Wysyłanie wiadomości do kolejki
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

// Funkcja do zwracania losowego wypieku
Wypieki losuj_wypiek() {
    Wypieki wypiek;
    long wybor = losuj_liczbe(1,15);  // Losuje wypiek spośród 15 dostępnych produktów

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
    wypiek.liczba_sztuk = losuj_liczbe(1,20);
    wypiek.cena = wypiek.cena * wypiek.liczba_sztuk;

    return wypiek;
}