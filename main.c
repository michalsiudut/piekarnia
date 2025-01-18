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

int losuj_liczbe(int min, int max) {
    return rand() % (max - min + 1) + min;
}


int main() {

    srand(time(NULL));
    printf("WITAMY W NASZEJ PIEKARNI\n");

    pid_t piekarz_pid = fork();
    if (piekarz_pid == 0) {
        execl("./piekarz", "piekarz", NULL); // Uruchomienie programu piekarza
        perror("Błąd przy uruchamianiu piekarza");
        exit(1);
    }

    pid_t klienty_pids[NUM_CLIENTS];
    sleep(3); // dajemy piekarzowi przed otwarciem piekarni upiec pare wypiekow
    for (int i = 0; i < NUM_CLIENTS; i++) {
        klienty_pids[i] = fork();

        if (klienty_pids[i] == 0) {
            execl("./klient", "klient", NULL); // Uruchomienie programu klienta
            perror("Błąd przy uruchamianiu klienta");
            exit(1);
        }
        int randomowy_czas_przyjscia_klientow = losuj_liczbe(1,5); // od 1 - 5 sekund przychodzi nowy klient
        sleep(randomowy_czas_przyjscia_klientow); //przychodzenie klientow o losowych czasach
    }
    /*
    // Uruchomienie procesu klienta
    pid_t klient_pid = fork();
    if (klient_pid == 0) {
        execl("./klient", "klient", NULL); // Uruchomienie programu klienta
        perror("Błąd przy uruchamianiu klienta");
        exit(1);
    }

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
    for (int i = 0; i < NUM_CLIENTS; i++) {
        waitpid(klienty_pids[i], NULL, 0); // Czekanie na zakończenie każdego klienta
    }
    waitpid(piekarz_pid, NULL, 0);   // Czekanie na piekarza
    printf("Symulacja zakończona.\n");
    return 0;
}
