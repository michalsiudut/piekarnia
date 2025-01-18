#ifndef COMMON_H
#define COMMON_H

#define NUM_PRODUCTS 15 // Liczba różnych produktów

#define CZAS_WYPIEKU 1 //czas wypieku
#define MAX_WYPIEKI 4 //maxymalna ilosc wypiekow na raz
// Klucze 
#define KEY_SHM 1234    // pamięc dzielona
#define KEY_SEM 5678    // semafory
#define KEY_MSG 3121    // kolejka kom


#define MAX_SZTUKI 15 // maksymalna ilosc wypiekow na podajniku

#define NUM_CLIENTS 100  // Liczba klientów
#define SHM_SIZE 1024   // Rozmiar pamięci dzielonej 



typedef struct {
    long mtype; 
    char nazwa[20];        // nazwa wypieku
    int liczba_sztuk;      // liczba sztuk
    int cena;              // cena
} Wypieki;


#endif // COMMON_H