#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include "definicje.h"

_Bool inwentaryzacja_aktywna = 0;
int iterator = 0;

// Funkcja wysyłająca sygnał ewakuacji do grupy procesów
void ewakuacja(pid_t pgid) {
    printf(CYAN"Kierownik: Ogłaszam ewakuację!"RESET"\n");
    usun_wszystko(pgid);
}
void inwentaryzacja(pid_t kasjer, pid_t piekarz) {
    printf(CYAN"Kierownik: Ogłaszam inwentaryzację!"RESET"\n");

    // Wysyłanie sygnału SIGUSR2 do procesów kasjera i piekarza w grupie procesów
    if (kill(kasjer, SIGUSR2) == -1) {
        perror(CYAN"Nie udało się wysłać sygnału SIGUSR2 do grupy procesów!"RESET);
        exit(1);
    }
    
    if (kill(piekarz, SIGUSR2) == -1) {
        perror(CYAN"Nie udało się wysłać sygnału SIGUSR2 do grupy procesów!"RESET);
        exit(1);
    }

    inwentaryzacja_aktywna = 1;
    
    printf(CYAN"Przeprowadzono inwentaryzację."RESET"\n");
}

void handle_sigterm(int sig) {
    printf(CYAN"Kierownik: Otrzymałem SIGTERM, zaczynam kończenie pracy.\n"RESET);

    // Uzyskanie dostępu do semaforów
    int semid = semget(KEY_SEM, 16, 0666);
    if (semid == -1) {
        perror(CYAN"Błąd przy uzyskiwaniu dostępu do zbioru semaforów"RESET);
        exit(1);
    }

    iterator = 0;

    // Przechodzenie przez wszystkie 16 semaforów
    for (int i = 0; i < 16; i++) {
        int wartosc_semafora = semctl(semid, i, GETVAL);
        if (wartosc_semafora == -1) {
            perror(CYAN"Błąd odczytu wartości semafora"RESET);
            exit(1);
        }
        iterator += (15 - wartosc_semafora); // Ilość wypieków na taśmie
    }

    // Podsumowanie
    printf(CYAN"[INWENTARYZACJA] Wypisuje ilość produktów na taśmach przez kierownika: %d\n"RESET, iterator);

    // Usunięcie zbioru semaforów
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror(CYAN"Błąd przy usuwaniu zbioru semaforów"RESET);
    } else {
        printf(CYAN"Zbiór semaforów został usunięty.\n"RESET);
    }

    // Zakończenie procesu kierownika
    exit(0);
}



void usun_wszystko(pid_t pgid) {
    printf(CYAN"Zamykam piekarnię i sprzątam zasoby..."RESET"\n");
    // uzyskanie dostepu do semaforow
    int semid = semget(KEY_SEM, 1, 0666);
    // uzyskanie dostepu do semaforw od wchodzenia do piekarni
    int semid2 = semget(SEM_KEY_DO_SKLEPU, 1, 0666);
    // uzyskanie dostepu do kolejki
    int msgid = msgget(KEY_MSG, 0666);
    // uzyskanie dostepu do kolejki 2
    int msgid2 = msgget(KEY_MSG_KLIENT_KASJER, 0666);
    if (msgid < 0) {
        perror(CYAN"Błąd przy uzyskiwaniu dostępu do kolejki"RESET);
        exit(1);
    }

    // usuwanie kolejki
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror(CYAN"Błąd przy usuwaniu kolejki"RESET);
    } else {
        printf(CYAN"Kolejka komunikatów została usunięta."RESET"\n");
    }
    // usuwanie 2 kolejki
     if (msgctl(msgid2, IPC_RMID, NULL) == -1) {
        perror(CYAN"Błąd przy usuwaniu kolejki"RESET);
    } else {
        printf(CYAN"Kolejka komunikatów (klient kasjer) została usunięta."RESET"\n");
    }
    // usuwanie semaforow
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror(CYAN"Błąd przy usuwaniu semafora"RESET);
        exit(1);
    }
    if (semctl(semid2, 0, IPC_RMID) == -1) {
        perror(CYAN"Błąd przy usuwaniu semafora"RESET);
        exit(1);
    }


    printf(" "RESET);
    if (kill(-pgid, SIGUSR1) == -1) {
        perror(CYAN"Nie udało się wysłać sygnału do grupy procesów"RESET);
        exit(1);
    }

    exit(0);
}

int main(int argc, char *argv[]) {

    signal(SIGTERM, handle_sigterm);

    pid_t kasjer_pid = atoi(argv[2]); // Konwersja kasjer_pid z argumentu
    pid_t piekarz_pid = atoi(argv[3]); // Konwersja piekarz_pid z argumentu

    pid_t pgid = atoi(argv[1]); // Odczytanie PID grupy procesów z argumentów
    printf(CYAN"Siema, jestem kierownikiem piekarni! Monitoruję grupę procesów GID: %d"RESET"\n", pgid);
    printf(CYAN"kasjer: %d, piekarz: %d"RESET"\n", kasjer_pid, piekarz_pid);

    sleep(25); 
    // ewakuacja(pgid); // Wywołanie ewakuacji
    inwentaryzacja(kasjer_pid,piekarz_pid);
    pause();


    return 0;
}