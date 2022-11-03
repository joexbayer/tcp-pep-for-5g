cmd_/home/joebayer/Documents/uio-master-joeba/sysinter/sysmodule.o := ld -m elf_x86_64 -z noexecstack --no-warn-rwx-segments   -r -o /home/joebayer/Documents/uio-master-joeba/sysinter/sysmodule.o @/home/joebayer/Documents/uio-master-joeba/sysinter/sysmodule.mod  ; ./tools/objtool/objtool  --hacks=jump_label  --hacks=noinstr  --ibt   --orc  --retpoline  --rethunk  --sls   --static-call  --uaccess  --link  --module  /home/joebayer/Documents/uio-master-joeba/sysinter/sysmodule.o

/home/joebayer/Documents/uio-master-joeba/sysinter/sysmodule.o: $(wildcard ./tools/objtool/objtool)
