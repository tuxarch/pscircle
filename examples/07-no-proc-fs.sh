#!/bin/bash

set -e

lmem=$(free -h | awk '/^Mem/ {print $3" / "$2}')
pmem=$(free -b | awk '/^Mem/ {print $3/$2}')
lcpu=$(uptime | awk -F'[a-z]: ' '{ print $2}')

if command -v mpstat >/dev/null ; then
	pcpu=$(mpstat -u 1 1 | awk '/Average:/{print (100-$12)/100}')
elif command -v sar >/dev/null ; then
	pcpu=$(sar -u  1 1 | awk '/Average:/{print (100-$8)/100}')
else
	echo Can not get CPU utilization: 'sar' or 'mpstat' are not installed
	pcpu=0.42
fi

ps -e -o pid,ppid,pcpu,rss,comm --no-headers | pscircle \
	--stdin=true \
	--memlist-bar-value="$pmem" \
	--memlist-label="$lmem" \
	--cpulist-bar-value="$pcpu" \
	--cpulist-label="$lcpu" 
