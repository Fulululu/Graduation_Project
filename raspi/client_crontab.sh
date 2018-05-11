#!/bin/bash

cd /home/pi/design
python3 myclient.py
while true
do
    echo 'awake'
    linenum=`ps -ef|grep 'python3 myclient.py'|wc -l`
    if [ $linenum -eq 1 ]; then
	python3 myclient.py
    fi
    echo 'sleep 60s'
    sleep 60
done
