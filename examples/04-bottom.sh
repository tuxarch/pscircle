#!/bin/bash

set -e

pscircle \
	--background-color=fff \
	--link-color-min=aaa \
	--link-color-max=900 \
	--dot-color-min=0a0 \
	--dot-color-max=900 \
	--tree-font-color=444 \
	--toplists-font-color=444 \
	--toplists-pid-font-color=777 \
	--toplists-bar-background=999 \
	--toplists-bar-color=ddbf54 \
	--max-children=35 \
	--root-pid=1 \
	--tree-sector-angle=3.9269 \
	--tree-center=0:350 \
	--tree-rotate=true \
	--tree-rotation-angle=3.5342 \
	--cpulist-center=-250:700 \
	--memlist-center=250:700

