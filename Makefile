.PHONY: lib module tools server client pep

all: tools

new: clean lib module tools

lib:
	make -C lib/

module:
	make -C kmodule/

tools:
	gcc tools/client_file.c lib/pep.so -o client_file
	gcc tools/server_file.c -o server_file -lpthread

install:
	make -C kmodule install