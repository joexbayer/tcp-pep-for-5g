#!/bin/bash
#
# Author:Joe Bayer
#
# Runs a mixed flow test
#

pep="-p"

./client -m -s 1000 -i 10 $pep &
sleep 2
./client_file $pep;