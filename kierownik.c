#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <wait.h>
#include "definicje.h"

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
    
    printf(CYAN"Przeprowadzono inwentaryzację."RESET"\n");
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


    pid_t kasjer_pid = atoi(argv[2]); // Konwersja kasjer_pid z argumentu
    pid_t piekarz_pid = atoi(argv[3]); // Konwersja piekarz_pid z argumentu

    pid_t pgid = atoi(argv[1]); // Odczytanie PID grupy procesów z argumentów
    printf(CYAN"Siema, jestem kierownikiem piekarni! Monitoruję grupę procesów GID: %d"RESET"\n", pgid);
    printf(CYAN"kasjer: %d, piekarz: %d"RESET"\n", kasjer_pid, piekarz_pid);

    sleep(25); 
    //ewakuacja(pgid); // Wywołanie ewakuacji
    inwentaryzacja(kasjer_pid,piekarz_pid);
    return 0;
}