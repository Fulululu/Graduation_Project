#!/bin/bash

cd /home/ygf/design
python3 myserver_asy.py
while true
do
    linenum=`ps -ef|grep 'python3 myserver_asy.py'|wc -l`
    if [ $linenum -eq 1 ]; then
	python3 myserver_asy.py
    fi
    sleep 60
done
