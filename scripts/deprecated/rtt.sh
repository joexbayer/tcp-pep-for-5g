#!/bin/bash
#
# Author:Joe Bayer
#
# Runs a mixed flow test
#

pep=""

./client_file $pep &
sleep 2
./client -m -s 5000 -i 1 $pep &