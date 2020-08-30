#!/bin/bash

picDir=$(find /private/var/folders/ -name com.apple.desktoppicture 2>/dev/null)

while true
do
    # using UUID so macOS reconizes it as a new file when you set it as a wallpaper
    pic=$(uuidgen | tr -d '\n')
    cpu_usage=$(printf %.f $(ps -A -o %cpu | awk '{ cpu += $1} END {print cpu}' | head -c 2)) # sum of all processes CPU usage
    mem_usage=$((100 - $(memory_pressure | tail -c 4 | grep -o '[0-9]*'))) # memory pressure (/100)
    ps -e -c -o pid=,ppid=,pcpu=,rss=,comm= \
        | pscircle --stdin=true \
        --root-pid=1 \ # defines launchd as root process of the tree
        --cpulist-label="${cpu_usage} %" \
        --memlist-label="${mem_usage} %" \
        --cpulist-bar-value="$(bc <<< "scale=2 ; $cpu_usage / 100")" \ # value between 0 and 1
        --memlist-bar-value="$(bc <<< "scale=2 ; $mem_usage / 100")" \

        --memlist-show=true \
        --cpulist-show=true \
        --output="/tmp/${pic}"

    osascript -e 'tell application "Finder" to set desktop picture to POSIX file "/tmp/'"$pic"'" '

    sleep 15

    rm /tmp/$pic $picDir/*.png 2>/dev/null

done
