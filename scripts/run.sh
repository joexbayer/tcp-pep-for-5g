#!/bin/bash
#
# Author:Joe Bayer
#
# Tests step function traffic control
# Assumes PEP is running!
#

# run server
#ssh joe@localhost -p 5556 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; make tools; ./server_file"
# tc on pep 1mb
#ssh -t joe@localhost -p 5557 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; sh scripts/tcconfig20ms.sh;"
# run client
ssh joe@localhost -p 5555 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; ./client_file"
# update pep tc to 10mb
ssh -t joe@localhost -p 5557 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; sudo sh scripts/tcstep.sh"