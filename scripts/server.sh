ssh joe@localhost -p 5556 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; make tools; ./server_file"
