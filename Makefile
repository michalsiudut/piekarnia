# Kompilator i flagi
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Pliki źródłowe
SRCS = main.c piekarz.c kasjer.c klient.c kierownik.c

# Pliki wykonywalne (każdy plik .c dostaje odpowiadający plik wykonywalny)
EXES = $(SRCS:.c=)

# Reguła domyślna
all: $(EXES)

# Kompilacja każdego pliku .c do pliku wykonywalnego
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Czyszczenie plików wykonywalnych
clean:
	rm -f $(EXES)

