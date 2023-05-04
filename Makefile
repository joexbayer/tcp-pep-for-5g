.PHONY: lib module tools server client pep

all: tools

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

vms:
	VBoxManage list runningvms

stopvms:
	VBoxManage controlvm Client poweroff
	VBoxManage controlvm Server poweroff
	VBoxManage controlvm Router poweroff

resetvms:
	VBoxManage controlvm Client reset
	VBoxManage controlvm Server reset
	VBoxManage controlvm Router reset

startvms:
	VBoxManage startvm Client --type headless
	VBoxManage startvm Server --type headless
	VBoxManage startvm Router --type headless

sshclient:
	ssh joe@localhost -p 5555 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no 
sshserver:
	ssh joe@localhost -p 5556 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no 
sshpep:
	ssh joe@localhost -p 5557 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no

resetpep:
	VBoxManage controlvm Router reset

help:
	@echo VMS:
	@echo  - vms - list of all vms
	@echo  - startvms - start all vms
	@echo  - stopvms - stop all vms
	@echo 
	@echo SSH:
	@echo  - sshclient - ssh into client
	@echo  - sshserver - ssh into server
	@echo  - sshpep -ssh into pep
	@echo 
	@echo Scripts:
	@echo  - server - setup server
	@echo  - client - setup client
	@echo  - pep - setup pep

server:
	sh scripts/server.sh
client:
	sh scripts/client.sh
pep:
	sh scripts/pep.sh


mininet:
	sudo mn --custom ./topology.py --topo=mytopo -x

clean:
	make -C lib/ clean
	make -C kmodule/ clean
	rm -f client
	rm -f server
	rm -f client_file
	rm -f server_file
