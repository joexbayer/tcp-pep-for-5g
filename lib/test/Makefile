CC ?= gcc

SRC_DIR = .
CFLAGS += -Wall -Wextra -std=gnu99 -I../include
LDFLAGS += -L.. -Wl,-rpath=..

INCS = ../src/tlv.o

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)
OUTPUTS = $(SRC:.c=.bin)

all: $(OUTPUTS)

%.bin: $(OBJ)
	$(CC) $(@:.bin=.o) $(LDFLAGS) $(INCS) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(SRC_DIR)/*.o *.bin

run-tests_valgrind:
	@for f in *.bin; do valgrind --leak-check=full --show-leak-kinds=all ./$$f; echo; done

run-tests:
	@for f in *.bin; do ./$$f; echo; done
