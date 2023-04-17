ssh joe@localhost -p 5555 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; make tools; ./client_file"
