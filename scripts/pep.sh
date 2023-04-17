ssh -t joe@localhost -p 5557 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; sh tcconfig.sh; cd kmodule; make clean && make && make install; make live-logs"
