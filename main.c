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
// Funkcja czyszcząca zasoby
void cleanup(int shmid, int semid, int msgid) {
    shmctl(shmid, IPC_RMID, NULL); // Usunięcie pamięci dzielonej
    semctl(semid, 0, IPC_RMID);   // Usunięcie semaforów
    msgctl(msgid, IPC_RMID, NULL); // Usunięcie kolejki komunikatów
    printf("Wszystkie zasoby zostały wyczyszczone.\n");
}

int main() {
    int shmid, semid, msgid;
    pid_t piekarz_pid, kierownik_pid, kasjer_pid;
    pid_t klienty_pids[NUM_CLIENTS];

    // Inicjalizacja pamięci dzielonej
    shmid = shmget(KEY_SHM, 1024, IPC_CREAT | 0666); // 1024 bajty pamięci
    if (shmid < 0) {
        perror("Błąd przy tworzeniu pamięci dzielonej");
        exit(1);
    }

    // Inicjalizacja semaforów
    semid = semget(KEY_SEM, 1, IPC_CREAT | 0666); // Jeden semafor
    if (semid < 0) {
        perror("Błąd przy tworzeniu semaforów");
        shmctl(shmid, IPC_RMID, NULL);
        exit(1);
    }
    semctl(semid, 0, SETVAL, 1); // Ustawienie początkowej wartości semafora na 1

    // Inicjalizacja kolejki komunikatów
    msgid = msgget(KEY_MSG, IPC_CREAT | 0666);
    if (msgid < 0) {
        perror("Błąd przy tworzeniu kolejki komunikatów");
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
        exit(1);
    }

    // Uruchomienie procesu piekarza
    piekarz_pid = fork();
    if (piekarz_pid == 0) {
        execl("./piekarz", "piekarz", NULL); // Uruchomienie programu piekarza
        perror("Błąd przy uruchamianiu piekarza");
        exit(1);
    }

    // Uruchomienie procesu kierownika
    kierownik_pid = fork();
    if (kierownik_pid == 0) {
        execl("./kierownik", "kierownik", NULL); // Uruchomienie programu kierownika
        perror("Błąd przy uruchamianiu kierownika");
        exit(1);
    }

    // Uruchomienie procesu kasjera
    kasjer_pid = fork();
    if (kasjer_pid == 0) {
        execl("./kasjer", "kasjer", NULL); // Uruchomienie programu kasjera
        perror("Błąd przy uruchamianiu kasjera");
        exit(1);
    }

    // Uruchomienie procesów klientów
    for (int i = 0; i < NUM_CLIENTS; i++) {
        klienty_pids[i] = fork();
        if (klienty_pids[i] == 0) {
            execl("./klient", "klient", NULL); // Uruchomienie programu klienta
            perror("Błąd przy uruchamianiu klienta");
            exit(1);
        }
    }

    // Czekanie na zakończenie procesów
    for (int i = 0; i < NUM_CLIENTS; i++) {
        waitpid(klienty_pids[i], NULL, 0); // Czekanie na zakończenie każdego klienta
    }
    waitpid(piekarz_pid, NULL, 0);   // Czekanie na piekarza
    waitpid(kierownik_pid, NULL, 0); // Czekanie na kierownika
    waitpid(kasjer_pid, NULL, 0);    // Czekanie na kasjera

    // Czyszczenie zasobów
    cleanup(shmid, semid, msgid);

    printf("Symulacja zakończona.\n");
    return 0;
}
