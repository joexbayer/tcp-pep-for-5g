.PHONY: lib module 

all: clean lib module tools

lib:
	make -C lib/

module:
	make -C kmodule/

tools:
	gcc client.c lib/pep.so -o client
	gcc server.c -o serv

install:
	make -C kmodule install


clean:
	make -C lib/ clean
	make -C kmodule/ clean
	rm -f client
	rm -f server
