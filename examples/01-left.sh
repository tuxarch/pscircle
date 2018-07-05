#!/bin/bash

set -e

pscircle \
	--root-pid=1 \
	--max-children=35 \
	--tree-sector-angle=3.1415 \
	--tree-rotate=true \
	--tree-rotation-angle=1.5708 \
	--tree-center=-1580:0 \
	--cpulist-center=300:0 \
	--memlist-center=800:0

