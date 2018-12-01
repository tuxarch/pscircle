#!/bin/bash

while true
do
	# using UUID so macOS reconizes it as a new file when you set it as a wallpaper
	s=$(uuidgen | tr -d '\n')

	ps -e -c -o pid=,ppid=,pcpu=,rss=,comm= | pscircle --stdin=true --memlist-show=false --cpulist-show=false --output="/tmp/${s}"

	osascript -e 'tell application "System Events" to set picture of every desktop to ("/tmp/'"$s"'" as POSIX file as alias)'

	rm /tmp/$s

	sleep 15
done
