CC     = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
SRC    = main.c \
         src/fila.c \
         src/estacionamento.c \
         src/display.c

all:
	$(CC) $(SRC) $(CFLAGS) -o estacionamento

clean:
	rm -f estacionamento
