#ifndef COMMON_H
#define COMMON_H

#define NUM_PRODUCTS 15 // Liczba różnych produktów (P > 10)

#define CZAS_WYPIEKU 2 //czas wypieku
// Klucze IPC
#define KEY_SHM 1234    // Klucz do pamięci dzielonej
#define KEY_SEM 5678    // Klucz do semaforów
#define KEY_MSG 8721    // Klucz do kolejki komunikatów

// Parametry piekarni
#define NUM_CLIENTS 10  // Liczba klientów
#define SHM_SIZE 1024   // Rozmiar pamięci dzielonej (w bajtach)

// Struktura dla kolejki komunikatów
typedef struct {
    long mtype; 
    char nazwa[20];        // nazwa wypieku
    int liczba_sztuk;      // liczba sztuk
    int cena;              // cena
} Wypieki;

#endif // COMMON_H
