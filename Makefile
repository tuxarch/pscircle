.PHONY: build install

build:
	mkdir -p build
	cd build; meson .. && ninja && sudo ninja install

