all: clean
	make -C lib/
	make -C kmodule/
	gcc client.c lib/pep.so -o client
	gcc server.c -o server

install:
	make -C kmodule install


clean:
	make -C lib/ clean
	make -C kmodule/ clean
	rm client
	rm server