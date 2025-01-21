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


void sem_op(int sem_id, int op) {
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = op;
    sops.sem_flg = 0;
    if (semop(sem_id, &sops, 1) == -1) {
        perror(RED"Błąd operacji na semaforze"RESET);
        exit(1);
    }
}



int main() {

    srand(time(NULL));
    printf(RED "WITAMY W NASZEJ PIEKARNI"RESET"\n");
    
    pid_t main_pid = getpid(); // PID procesu głównego
    if (setpgid(main_pid, 0) == -1) {
        perror("Błąd podczas tworzenia grupy procesów");
        exit(1);
    }

    pid_t piekarz_pid = fork();
    if (piekarz_pid == 0) {
        setpgid(0, main_pid);
        execl("./piekarz", "piekarz", NULL); // Uruchomienie programu piekarza
        perror("Błąd przy uruchamianiu piekarza");
        exit(1);
    }

    pid_t kierownik_pid = fork();
    if (kierownik_pid == 0) {
        setpgid(0, main_pid);
        char main_pid_str[10];
        snprintf(main_pid_str, sizeof(main_pid_str), "%d", main_pid); // Konwersja main_pid na string
        execl("./kierownik", "kierownik", main_pid_str, NULL); // Przekazanie main_pid jako argument
        perror("Błąd przy uruchamianiu kierownika");
    }

    int msgid_kasjer = msgget(KEY_MSG_KLIENT_KASJER, IPC_CREAT | IPC_EXCL | 0666); // twprzenie kolejki kom z klienta do kasjera
    if (msgid_kasjer < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów");
        exit(1);
    }


    pid_t kasjer_pid = fork();
    if (kasjer_pid == 0) {
        setpgid(0, main_pid);
        execl("./kasjer", "kasjer", NULL); // Uruchomienie programu kasjera
        perror(RED"Błąd przy uruchamianiu kasjera"RESET);
        exit(1);
    }


     // Inicjalizacja semafora
    int sem_id = semget(SEM_KEY_DO_SKLEPU, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror(RED"Błąd tworzenia semafora"RESET);
        exit(1);
    }

    // Ustawienie wartości początkowej semafora
    if (semctl(sem_id, 0, SETVAL, MAX_KLIENTOW) == -1) {
        perror(RED"Błąd ustawiania wartości semafora"RESET);
        exit(1);
    }


    sleep(2); // dajemy piekarzowi przed otwarciem piekarni upiec pare wypiekow
    time_t start_time = time(NULL);
    int czas_trwania = 30; // Symulacja trwa 30 sekund
    pid_t klienty_pids[NUM_CLIENTS];
    int i = 0;

    while (time(NULL) - start_time < czas_trwania) {
        klienty_pids[i] = fork();
        if (klienty_pids[i] == 0) {
            setpgid(0, main_pid);
            sem_op(sem_id, -1); // wchodzenie klienta do sklepu
            execl("./klient", "klient", NULL); // Uruchomienie programu klienta
            perror(RED"Błąd przy uruchamianiu klienta"RESET);
            exit(1);
        }
        int randomowy_czas_przyjscia_klientow = losuj_liczbe(1, 2); // Nowi klienci co 1–2 sekund
        sleep(randomowy_czas_przyjscia_klientow);
        i++;
    }
    printf(RED"Zamknięto wejście dla kientów.\n" RESET);
    printf(RED"Czekanie na opuszczenie piekarni przez klientów\n" RESET);
    
    for (int j = 0; j < i; j++) {
        waitpid(klienty_pids[j], NULL, 0);
    }


    // Wysłanie sygnału do piekarza, aby zakończył pracę
    kill(piekarz_pid, SIGTERM);
    // jak i do kasjera
    kill(kasjer_pid, SIGTERM);
    // czekamy na nich
    waitpid(kasjer_pid, NULL, 0);
    waitpid(piekarz_pid, NULL, 0);   // Czekanie na piekarza
    waitpid(kierownik_pid, NULL, 0);   // Czekanie na kieronwika

    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror(RED"Błąd usuwania semafora"RESET);
        exit(1);
    }
    
    printf(RED"Symulacja zakończona.\n" RESET);
    return 0;
}