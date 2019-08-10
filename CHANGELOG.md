# 1.3.1 - 2019.08.10

## Added

* KDE Example and a link to the guide at reddit
* native build (-march=native) is now enabled only for realease build mode

## Fixed

* fixed assert() in realease mode
* fixed assert() when list lablel is empty
* fixed GCC warnings on string truncation

# 1.3 - 2018.12.01

## Added

* Support for background pattern images
* Option for displaying root process (`--show-root`)
* Option for changing label angle of the root noe (`--root-label-angle`)
* Option for merging the links comming from the root node (`--root-link-sector`)
* Examples of MacOS

## Updated

* Labels are always drawn on top of the links
* Fixed systemd service example

# 1.2 - 2018.07.20

## Added

* Multiple circles radii support (e.g `--tree-radius-increment=30,40,50`)
* Option for hiding tree levels (`--hide-top-levels=1`)
* GNOME example
* `--version` option

# 1.1 - 2018.07.06

## Added

* `--collapse-threads` option
* Gentoo ebuild
* Pywal script

## Updated

* Readme: descriptions and guides, fixed typos

## Removed

* `-mavx` compilation option,
