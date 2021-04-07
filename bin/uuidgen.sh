#!/usr/bin/bash
# Brian Chrzanowski
# 2021-04-07 10:22:50
#
# call the system uuidgen more than once

[ -z $1 ] && AMT=1 || AMT=$1

for i in $(seq 1 $AMT); do
	uuidgen
done

