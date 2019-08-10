#!/bin/bash

# this script was submitted to me via email (by vov4ikpa) with the following instructions:

# 1. In Autostart menu and add the script.
# 2. Enter "configure desktop" and choose slide show
# 3. Add the directory (/tmp/wallpaper) the script is using to generate wallpapers.

mkdir -p /tmp/wallpaper

i=1
while [ true ]
do
	pscircle --output=/tmp/wallpaper/wall${i}.png
	i=$(( ${i}*-1 ))
	sleep 10
done
