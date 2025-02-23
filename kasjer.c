#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>
#include "definicje.h"

int iterator = 0;
_Bool inwentaryzacja_aktywna = 0;
pid_t pid_kasjera;

void handle_sigusr2(int sig)
{
    printf(BLUE "Kasjer: Otrzymałem sygnał inwentaryzacji (SIGUSR2)." RESET "\n");
    inwentaryzacja_aktywna = 1;
}

void sem_op(int sem_id, int op)
{
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = op;
    sops.sem_flg = 0;
    if (semop(sem_id, &sops, 1) == -1)
    {
        perror(BLUE "Błąd operacji na semaforze" RESET);
        exit(1);
    }
}

void handle_sigterm(int sig)
{

    // uzyskanie dostepu do kolejki
    int msgid = msgget(KEY_MSG_KLIENT_KASJER, 0600);
    if (msgid < 0)
    {
        perror(BLUE "Błąd przy uzyskiwaniu dostępu do kolejki" RESET);
        exit(1);
    }

    // usuwanie kolejki

    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror(BLUE "Błąd przy usuwaniu kolejki" RESET);
    }
    else
    {
        printf(BLUE "Kolejka komunikatów klient - kasjer została usunięta." RESET "\n");
    }

    if (inwentaryzacja_aktywna)
    {
        printf(BLUE "[INWENTARYZACJA] Wypisuje ilość produktów sprzedanych przez kasjera [%d]: %d\n" RESET, pid_kasjera, iterator);
    }

    printf(BLUE "Kasjer: Zamykam kasę." RESET "\n");

    exit(0);
}

int main()
{
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR2, handle_sigusr2); // Przypisanie obsługi SIGUSR2
    pid_kasjera = getpid();
    int sem_id = semget(SEM_KEY_DO_SKLEPU, 1, 0);
    if (sem_id == -1)
    {
        perror(BLUE "Błąd uzyskiwania dostępu do semafora" RESET);
        exit(1);
    }

    int msgid = msgget(KEY_MSG_KLIENT_KASJER, 0600);
    if (msgid < 0)
    {
        perror(BLUE "Błąd przy uzyskiwaniu dostępu do kolejki komunikatów@@@@" RESET);
        exit(1);
    }

    Wypieki wypieki; // Struktura do odbioru wiadomości

    ssize_t msg_rcv_result;
    while (1)
    {
        // Odbieranie wiadomości z kolejki
        while ((msg_rcv_result = msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0, 0)) == -1 && errno == EINTR)
        {
            // Czekamy na zakończenie sygnału i próbujemy ponownie
            continue;
        }

        if (msg_rcv_result == -1)
        {
            perror("Błąd msgrcv");
            exit(1);
        }
        iterator += wypieki.liczba_sztuk;
        Wypieki wypieki_tab[20];
        wypieki_tab[0] = wypieki;
        for (int i = 1; i < wypieki.liczba_obrotow; i++)
        {
            if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0, 0) == -1)
            {
                perror(BLUE "Błąd msgrcv" RESET);
                break;
            }
            // printf("\t\tWypiek: %s | typ wypieku: %d | Ilość: %d | Cena: %d | pid: %d\n", wypieki.nazwa,wypieki.mtype, wypieki.liczba_sztuk, wypieki.cena, wypieki.pid_klienta);
            wypieki_tab[i] = wypieki;
            iterator += wypieki.liczba_sztuk;
        }

        // Przetwarzanie zakończone
        int sem_value = semctl(sem_id, 0, GETVAL);
        printf(BLUE "Ilość miejsc w piekarni: %d" RESET "\n", sem_value);
        printf(BLUE "PARAGON DLA KLIENTA: %d" RESET "\n", wypieki.pid_klienta);
        int suma_cen = 0;
        for (int i = 0; i < wypieki.liczba_obrotow; i++)
        {
            printf(BLUE "Rodzaj: %s, Liczba sztuk: %d, cena: %d" RESET "\n", wypieki_tab[i].nazwa, wypieki_tab[i].liczba_sztuk, wypieki_tab[i].cena);
            suma_cen += wypieki_tab[i].cena;
        }
        printf(BLUE "DLUZNOSC CALKOWAITA: %d   | Wystawiono przez kasjera: %d" RESET "\n", suma_cen, pid_kasjera);
        sem_op(sem_id, 1);

        usleep(100000);
        ;
    }
    return 0;
}