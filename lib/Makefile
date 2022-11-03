CC ?= gcc

# source directory and output name
SRC_DIR = src
OUTPUT = pep.so

# flags
CFLAGS += $(CONFIG) -Wall -Wextra -fPIC -std=gnu99 -O2 -Iinclude
LDFLAGS += -shared

# source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = src/library.o

$(OUTPUT): $(OBJ) src/tlv.o
	$(CC) $(OBJ) $(LDFLAGS) src/tlv.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ) $(OUTPUT) src/tlv.o

tests:
	make -C ./test/ clean
	make -C ./test/
	make -C ./test/ run-tests
