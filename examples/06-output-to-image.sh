#!/bin/bash

set -e

output=pscircle.png

pscircle --output=$output

if command -v feh >/dev/null; then
	feh --bg-fill $output
	rm $output
fi

