#!/bin/bash
#
# Author:Joe Bayer
#
# Running a Step function test.
#
sh scripts/tcconfig20ms.sh; 
sudo echo "Starting";
sleep 9; echo "Step 1";
sh scripts/tcconfig20ms1.sh; 
sleep 2; echo "Step 2";
sh scripts/tcconfig20ms2.sh;