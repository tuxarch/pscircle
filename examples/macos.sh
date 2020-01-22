#!/bin/bash

while true
do
	# using UUID so macOS reconizes it as a new file when you set it as a wallpaper
	pic=$(uuidgen | tr -d '\n')

	ps -e -c -o pid=,ppid=,pcpu=,rss=,comm= | \
		pscircle --stdin=true \
				 --memlist-show=false \
				 --cpulist-show=false \
				 --output="/tmp/${pic}"

	osascript -e 'tell application "Finder" to set desktop picture to POSIX file "/tmp/'"$pic"'" '

	sleep 10

	rm /tmp/$pic

done