#!/bin/bash
#
# Author:Joe Bayer
#
# Runs a mixed flow test
#
./client -m -s 2000 -i 10 &
sleep 2
./client_file;