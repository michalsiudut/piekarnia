#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "definicje.h" // W tej bibliotece powinny być definicje struktur, np. Wypieki



// Funkcja odbierająca wiadomości z kolejki
void odbieraj_wiadomosc(int msgid) {
    Wypieki wypieki;
    
    // Odbieranie wiadomości z kolejki (blokujące)
    while (1) {
        if (msgrcv(msgid, &wypieki, sizeof(wypieki) - sizeof(long), 0, 0) == -1) {
            perror("Błąd przy odbieraniu wiadomości");
            break;
        }
        
        // Wypisywanie odebranych danych
        printf("Odebrano produkt: %s | Ilość: %d | Cena: %d\n", wypieki.nazwa,wypieki.liczba_sztuk,wypieki.cena);
    }
}

int main() {
    key_t key = KEY_MSG;  // Klucz, który powinien być taki sam jak w piekarzu
    int msgid;
    // Uzyskanie dostępu do tej samej kolejki komunikatów
    msgid = msgget(key, 0666);
    if (msgid < 0) {
        perror("Błąd przy uzyskiwaniu dostępu do kolejki komunikatów");
        exit(1);
    }

    printf("Odbiorca: Rozpoczynam odbiór wypieków.\n");

    // Odbieranie wiadomości z kolejki
    odbieraj_wiadomosc(msgid);

    return 0;
}
