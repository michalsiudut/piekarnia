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



int main() {
    printf("WITAMY W NASZEJ PIEKARNI\n");

    pid_t piekarz_pid = fork();
    if (piekarz_pid == 0) {
        execl("./piekarz", "piekarz", NULL); // Uruchomienie programu piekarza
        perror("Błąd przy uruchamianiu piekarza");
        exit(1);
    }

    // Uruchomienie procesu kierownika
    pid_t klient_pid = fork();
    if (klient_pid == 0) {
        execl("./klient", "klient", NULL); // Uruchomienie programu klienta
        perror("Błąd przy uruchamianiu klienta");
        exit(1);
    }
/*
    // Uruchomienie procesu kasjera
    pid_t kasjer_pid = fork();
    if (kasjer_pid == 0) {
        execl("./kasjer", "kasjer", NULL); // Uruchomienie programu kasjera
        perror("Błąd przy uruchamianiu kasjera");
        exit(1);
    }
     // Uruchomienie procesu kierownika
    pid_t kierownik_pid = fork();
    if (kierownik_pid == 0) {
        execl("./kierownik", "kierownik", NULL); // Uruchomienie programu kierownika
        perror("Błąd przy uruchamianiu kierownika");
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
*/
    waitpid(klient_pid, NULL, 0);    // Czekanie na kasjera
    waitpid(piekarz_pid, NULL, 0);   // Czekanie na piekarza
    printf("Symulacja zakończona.\n");
    return 0;
}
