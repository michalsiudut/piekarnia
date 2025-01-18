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

    sleep(3); // dajemy piekarzowi przed otwarciem piekarni upiec pare wypiekow
    time_t start_time = time(NULL);
    int czas_trwania = 30; // Symulacja trwa 30 sekund
    pid_t klienty_pids[NUM_CLIENTS];
    int i = 0;

    while (time(NULL) - start_time < czas_trwania) {
        klienty_pids[i] = fork();
        if (klienty_pids[i] == 0) {
            execl("./klient", "klient", NULL); // Uruchomienie programu klienta
            perror("Błąd przy uruchamianiu klienta");
            exit(1);
        }
        int randomowy_czas_przyjscia_klientow = losuj_liczbe(1, 5); // Nowi klienci co 1–5 sekund
        sleep(randomowy_czas_przyjscia_klientow);
        i++;
    }
    for (int j = 0; j < i; j++) {
        waitpid(klienty_pids[j], NULL, 0);
    }

    printf("Koniec czasu - zamykamy piekarnię.\n");
    fflush(stdout);

    // Wysłanie sygnału do piekarza, aby zakończył pracę
    kill(piekarz_pid, SIGTERM);
    
    waitpid(piekarz_pid, NULL, 0);   // Czekanie na piekarza
    printf("Symulacja zakończona.\n");
    return 0;
}