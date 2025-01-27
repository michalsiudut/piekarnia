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

int losuj_liczbe(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

void V(int semid, int i, int x)
{
    struct sembuf op;
    op.sem_num = x; // Semafor nr x
    op.sem_op = i;  // Zwiększ wartość semafora i
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1)
    {
        perror(YELLOW "Błąd przy operacji V()" RESET);
        exit(1);
    }
}

void odbieraj_wypiek(int msgid, int semid, int ile_rzeczy, int msgid_kasjer)
{

    Wypieki wypieki;
    Wypieki wypieki_klienta;
    pid_t pid_klienta = getpid();
    _Bool czy_odebrano = 1;
    _Bool czy_wystarczajaco = 1;
    Wypieki wypieki_tab[10];
    int z = 0;
    while (ile_rzeczy > 0)
    {
        wypieki_klienta = losuj_wypiek(); // losowanie wypieku klienta ktory chce zabrac
        sleep(1);                         // podchodzenied do tasmy i sprawdzanie czy sa wypieki

        // branie wypieku z tasmy
        if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), wypieki_klienta.mtype, IPC_NOWAIT) == -1)
        {
            if (errno == ENOMSG)
            {
                printf(YELLOW "KLIENT: %d Nie znaleziono wypieku, ktory klient chce kupic: %s" RESET "\n", pid_klienta, wypieki_klienta.nazwa);
                czy_odebrano = 0;
                wypieki.liczba_sztuk = 0;
                wypieki.mtype = 0;
            }
            else
            {
                perror(YELLOW "Błąd msgrcv" RESET);
            }
        }
        sleep(1); // symulacja odbierania wypieku przez klienta

        int wartosc_semafora = semctl(semid, wypieki.mtype, GETVAL);
        int ile_na_tasmie = 15 - wartosc_semafora;

        if (wartosc_semafora == -1)
        {
            perror(YELLOW "Błąd przy pobieraniu wartości semafora" RESET);
            exit(1);
        }
        // printf("\tile na tasmie: %d, a klient chcial: %d, TYP semafora: %d\n", ile_na_tasmie,wypieki_klienta.liczba_sztuk, wypieki.mtype);// WAZNE DO SPRAWDZNIAAAAAAAAAAA

        if (wypieki_klienta.liczba_sztuk <= ile_na_tasmie)
        {
            printf(YELLOW "KLIENT %d -----Odebrano produkt: %s | Ilość: %d | Cena: %d | TYP: %d" RESET "\n", pid_klienta, wypieki.nazwa, wypieki_klienta.liczba_sztuk, wypieki_klienta.cena, wypieki.mtype);
            wypieki_tab[z] = wypieki_klienta;
            z++;
        }
        else
        {
            printf(YELLOW "KLIENT %d ----Nie wystarczająco %s na taśmie" RESET "\n", pid_klienta, wypieki_klienta.nazwa);
            czy_wystarczajaco = 0;
        }

        if (czy_odebrano && czy_wystarczajaco)
        {
            V(semid, wypieki_klienta.liczba_sztuk, wypieki.mtype);
        }

        wartosc_semafora = semctl(semid, wypieki.mtype, GETVAL);
        ile_na_tasmie = 15 - wartosc_semafora;
        // printf("\tile na tasmie: %d\n", ile_na_tasmie);// WAZNE DO SPRAWDZANIAAAAAAAAAAAAAAAAAAA
        ile_rzeczy--;
        czy_odebrano = 1;
        czy_wystarczajaco = 1;
    }

    printf(YELLOW "ODEBRANE RZECZY PRZEZ KLIENTA----%d---------:" RESET "\n", pid_klienta);
    for (int i = 0; i < z; i++)
    {
        wypieki_tab[i].pid_klienta = pid_klienta;
        wypieki_tab[i].liczba_obrotow = z;
        printf(YELLOW "%s | %d | %d" RESET "\n", wypieki_tab[i].nazwa, wypieki_tab[i].liczba_sztuk, wypieki_tab[i].cena);
        if (msgsnd(msgid_kasjer, &wypieki_tab[i], sizeof(wypieki_tab[i]) - sizeof(long), 0) == -1)
        {
            perror(YELLOW "Błąd przy wysyłaniu tablicy wypieków do kasjera" RESET);
            exit(1);
        }
    }
    sleep(4); // Podchodzenie do kasy, płacenie za towar
}

int main()
{
    key_t key = KEY_MSG;
    int msgid, semid;
    int ile_rzeczy = 0;
    srand(time(NULL));

    ile_rzeczy = losuj_liczbe(2, 7);

    // Uzyskanie dostępu do semafora
    semid = semget(KEY_SEM, 1, 0);
    if (semid == -1)
    {
        perror(YELLOW "Błąd przy uzyskiwaniu dostępu do semafora" RESET);
        exit(1);
    }

    // uzyskiwanie dostepu do kolejki
    msgid = msgget(key, 0600);
    if (msgid < 0)
    {
        perror(YELLOW "Błąd przy uzyskiwaniu dostępu do kolejki komunikatów!!!!" RESET);
        exit(1);
    }
    // uzyskiwanie dostepu do kolejki klient- kasjer
    int msgid_kasjer = msgget(KEY_MSG_KLIENT_KASJER, 0600);
    if (msgid_kasjer < 0)
    {
        perror(YELLOW "Błąd przy uzyskiwaniu dostępu do kolejki komunikatów" RESET);
        exit(1);
    }
    // printf("------Klient: %d zaczyna robic zakupy\n", getpid());
    odbieraj_wypiek(msgid, semid, ile_rzeczy, msgid_kasjer);

    return 0;
}

Wypieki losuj_wypiek()
{
    Wypieki wypiek;
    long wybor = losuj_liczbe(1, 14); // losowanie od 0 do 14

    switch (wybor)
    {
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
    wypiek.liczba_sztuk = losuj_liczbe(1, 4); // od 1 do 4 sztuk
    wypiek.cena = wypiek.cena * wypiek.liczba_sztuk;

    return wypiek;
}