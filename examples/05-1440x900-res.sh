#!/bin/bash

set -e

output=pscircle.png

pscircle --output=$output \
	--max-children=60 \
	--output-width=1400 \
	--output-height=900 \
	--tree-radius-increment=90 \
	--dot-radius=3 \
	--link-width=1.3 \
	--tree-font-size=10 \
	--toplists-font-size=11 \
	--toplists-bar-width=30 \
	--toplists-row-height=15 \
	--toplists-bar-height=3 \
	--cpulist-center=400.0:-80.0 \
	--memlist-center=400.0:80.0

if command -v feh >/dev/null; then
	feh --bg-fill $output
	rm $output
fi

