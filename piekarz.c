#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "definicje.h"

Wypieki losuj_wypiek();

_Bool inwentaryzacja_aktywna = 0;
Wypieki historia_wpiekow[1000];
int iterator = 0;

void handle_sigusr2(int sig)
{
    printf(GREEN "Piekarz: Otrzymałem sygnał inwentaryzacji (SIGUSR2)." RESET "\n");
    inwentaryzacja_aktywna = 1;
}

void init_semaphore(int semid)
{
    for (int i = 0; i < 16; i++)
    {
        if (semctl(semid, i, SETVAL, MAX_SZTUKI) == -1)
        {
            perror(GREEN "Błąd przy inicjalizacji semafora" RESET);
            exit(1);
        }
    }
}
void P(int semid, int i, int x)
{
    struct sembuf op;
    op.sem_num = x; // Semafor nr 0
    op.sem_op = -i; // Zmniejsz wartość semafora
    op.sem_flg = 0;
    // printf("numer semafora: %d , pomniejszona wartosc smeafora: %d\n", x, i);
    while (semop(semid, &op, 1) == -1)
    {
        if (errno == EINTR)
        { // jesli sygnal SIGUSR1 lub SIGUSR2 przerywa to sprawdzamy czy to bylo to jak to to  dzialamy a jak nie to blad
            continue;
        }
        else
        {
            // Inny błąd
            perror("Błąd przy operacji P()");
            exit(1);
        }
    }
}

void usuniecie_kolejki(int sig)
{
    // uzyskanie dostepu do semaforow
    int semid = semget(KEY_SEM, 1, 0600);
    // uzyskanie dostepu do kolejki
    int msgid = msgget(KEY_MSG, 0600);
    if (msgid == -1)
    {
        perror(GREEN "Błąd przy uzyskiwaniu dostępu do kolejki" RESET);
        exit(1);
    }
    // usuwanie kolejki
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror(GREEN "Błąd przy usuwaniu kolejki" RESET);
    }
    else
    {
        printf(GREEN "Kolejka komunikatów została usunięta.\n" RESET);
    }
    /*
    // usuwanie semaforow
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror(GREEN"Błąd przy usuwaniu semafora"RESET);
        exit(1);
    }
    */
    if (inwentaryzacja_aktywna)
    {
        printf(GREEN "[INWENTARYZACJA] Wypisuje ilość produktów wyprodukowane przez piekarza: %d\n" RESET, iterator);
    }
    else
    {
        if (semctl(semid, 0, IPC_RMID) == -1)
        {
            perror(GREEN "Błąd przy usuwaniu semafora" RESET);
            exit(1);
        }
    }
    exit(0);
}

int losuj_liczbe(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

int main()
{
    int msgid, semid;
    Wypieki wypieki;

    // inicjalizacja generatora liczb losowych
    srand(time(NULL));
    signal(SIGINT, usuniecie_kolejki);
    signal(SIGTERM, usuniecie_kolejki);
    signal(SIGUSR2, handle_sigusr2); // Przypisanie obsługi SIGUSR2
    // uzyskanie dostepu do kolejki
    msgid = msgget(KEY_MSG, IPC_CREAT | IPC_EXCL | 0600);
    if (msgid < 0)
    {
        perror(GREEN "Błąd przy uzyskiwaniu dostępu do kolejki komunikatów" RESET);
        exit(1);
    }

    semid = semget(KEY_SEM, 16, IPC_CREAT | 0600);
    if (semid == -1)
    {
        perror(GREEN "Błąd przy tworzeniu semafora" RESET);
        printf(RED "errno: %d" RESET "\n", errno);
        printf("KEY_SEM: %d, liczba semaforów: %d\n", KEY_SEM, 16);
        exit(1);
    }
    init_semaphore(semid);

    printf(GREEN "Piekarz: Rozpoczynam produkcję wypieków.\n" RESET);
    while (1)
    {
        wypieki = losuj_wypiek();
        Wypieki wszystkie_wypieki[500];
        historia_wpiekow[iterator] = wypieki;
        P(semid, wypieki.liczba_sztuk, wypieki.mtype);
        // wysylanie losowego wypieku od kolejki
        if (msgsnd(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0) < 0)
        {
            perror(GREEN "Błąd przy wysyłaniu wiadomości" RESET);
            usuniecie_kolejki(SIGINT);
            break;
        }

        printf(GREEN "Piekarz: Wyprodukowano produkt %s w ilości %d w cenie %d." RESET "\n",
               wypieki.nazwa, wypieki.liczba_sztuk, wypieki.cena);

        usleep(200000);
        ; // 0.3 sekund na wypiek
        iterator += wypieki.liczba_sztuk;
        // printf("%d\n", iterator);
    }

    return 0;
}

// funkcja zwraca losowy wypiek
Wypieki losuj_wypiek()
{
    Wypieki wypiek;
    long wybor = losuj_liczbe(1, 14); // losowanie%

    switch (wybor)
    {
    case 0:
        strcpy(wypiek.nazwa, "Chleb pszenny");
        wypiek.cena = 3;
        break;
    case 1:
        strcpy(wypiek.nazwa, "Chleb zytni");
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
        strcpy(wypiek.nazwa, "Bułki maslane");
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
        strcpy(wypiek.nazwa, "Paczki");
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