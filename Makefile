.PHONY: lib module tools 

all: clean lib module tools

lib:
	make -C lib/

module:
	make -C kmodule/

tools:
	gcc tools/client.c lib/pep.so -o client
	gcc tools/server.c -o server
	gcc tools/client_file.c lib/pep.so -o client_file
	gcc tools/server_file.c -o server_file

install:
	make -C kmodule install


clean:
	make -C lib/ clean
	make -C kmodule/ clean
	rm -f client
	rm -f server
