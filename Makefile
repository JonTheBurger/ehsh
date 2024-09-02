all:
	cmake -S. -Bbuild
	cmake --build build

clean:
	cmake -E rm -rf build
