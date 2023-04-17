#!/bin/bash

# Objectives:
#  1. Run tcconfig.sh on router and start PEP
#  2. Start ./server on server
#  3. Setup initial tc for client.

# Assumes all 3 virtualbox devices are running.

# Server
ssh joe@localhost -p 5556 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; kill %1"
# Router
ssh -t joe@localhost -p 5557 -i ~/.ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba/kmodule; make uninstall"