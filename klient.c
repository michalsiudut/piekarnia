#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include "definicje.h"

Wypieki losuj_wypiek();

int losuj_liczbe(int min, int max) {
    return rand() % (max - min + 1) + min;
}

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

void odbieraj_wypiek(int msgid, int semid, int ile_rzeczy) {

    Wypieki wypieki;
    Wypieki wypieki_klienta;



    _Bool czy_odebrano = 1;
    _Bool czy_zgodna_ilosc = 0;

    //printf("......ile rzeczy: %d\n", ile_rzeczy);
    while (ile_rzeczy > 0) {
        wypieki_klienta = losuj_wypiek(); // losowanie wypieku klienta ktory chce zabrac
        //printf("/////Wylosowano wypiek nazwa:%s,    ilosc: %d\n", wypieki_klienta.nazwa, wypieki_klienta.liczba_sztuk);

        // branie wypieku z tasmy
        if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), wypieki_klienta.mtype,IPC_NOWAIT) == -1) {
            if (errno == ENOMSG) {
                printf("Nie znaleziono wypieku, ktory klient chce kupic: %s, o ilosci: %d\n", wypieki_klienta.nazwa, wypieki_klienta.liczba_sztuk);
                czy_odebrano = 0;
                wypieki.liczba_sztuk = 0;
            } else {
                perror("Błąd msgrcv");
            }
        }
        printf("Klient chce^^^ nazwa: %s, ilosc: %d, TYP: %d\n", wypieki_klienta.nazwa, wypieki_klienta.liczba_sztuk, wypieki_klienta.mtype);

        if(wypieki.liczba_sztuk == wypieki_klienta.liczba_sztuk){
            czy_zgodna_ilosc = 1;
            // printf("wypieki: %d, wypieki_klienta: %d\n", wypieki.liczba_sztuk, wypieki_klienta.liczba_sztuk);
        }
        sleep(1); // symulacja odbierania wypieku przez klienta
        if(czy_odebrano && czy_zgodna_ilosc){
            printf("-----Odebrano produkt: %s | Ilość: %d | Cena: %d | TYP: %d\n", wypieki.nazwa,wypieki.liczba_sztuk,wypieki.cena,wypieki.mtype);
            V(semid,wypieki.liczba_sztuk,wypieki.mtype);
            
        }else if(czy_odebrano){
            if (msgsnd(msgid, &wypieki, sizeof(Wypieki) - sizeof(long), 0) == -1) {
                perror("Błąd przy wysyłaniu wiadomości z powrotem");
            exit(1);  
            }
            printf("Wiadomosc zostala odeslana:%s, liczba sztuk: %d, TYP: %d\n",wypieki.nazwa, wypieki.liczba_sztuk,wypieki.mtype);
        }
        ile_rzeczy--;
        czy_odebrano = 1;
        czy_zgodna_ilosc = 0;
    }
}

int main() {
    key_t key = KEY_MSG;
    int msgid, semid;
    int ile_rzeczy = 0;
    srand(time(NULL));

    ile_rzeczy = losuj_liczbe(2,4);
    // printf("ile_rzeczy: %d", ile_rzeczy);
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

    printf("-------Klient: Zaczynam zakupy\n");
    odbieraj_wypiek(msgid, semid, ile_rzeczy);

    return 0;
}


Wypieki losuj_wypiek() {
    Wypieki wypiek;
    long wybor = losuj_liczbe(1,14); // losowanie od 0 do 14

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