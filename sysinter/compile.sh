sudo insmod sysmodule.ko sys_call_table_addr=0x$(sudo grep 'sys_call_table' /proc/kallsyms | cut -d' ' -f1)
