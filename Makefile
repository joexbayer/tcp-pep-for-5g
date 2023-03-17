.PHONY: lib module tools 

all: lib module tools

new: clean lib module tools

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

mininet:
	sudo mn --custom ./topology.py --topo=mytopo -x

clean:
	make -C lib/ clean
	make -C kmodule/ clean
	rm -f client
	rm -f server
	rm -f client_file
	rm -f server_file
