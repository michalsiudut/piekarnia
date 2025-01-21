#ifndef COMMON_H
#define COMMON_H

#define NUM_PRODUCTS 15 // Liczba różnych produktów
#define SEM_KEY_DO_SKLEPU 3112 //semafor trzymajacy max 5 klientow w sklepie

#define CZAS_WYPIEKU 1 //czas wypieku
#define MAX_WYPIEKI 4 //maxymalna ilosc wypiekow na raz
// Klucze 
#define KEY_SEM 1290    // semafory
#define KEY_MSG 3121    // kolejka kom
#define KEY_MSG_KLIENT_KASJER 3544    // kolejka kom kleint - kasjer

#define MAX_KLIENTOW 10

#define MAX_SZTUKI 15 // maksymalna ilosc wypiekow na podajniku

#define NUM_CLIENTS 100  // Liczba klientów
#define SHM_SIZE 1024   // Rozmiar pamięci dzielonej 



typedef struct {
    long mtype; 
    char nazwa[20];        // nazwa wypieku
    int liczba_sztuk;      // liczba sztuk
    int cena;              // cena
    int pid_klienta;
    int liczba_obrotow;
} Wypieki;

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"


#endif // COMMON_H