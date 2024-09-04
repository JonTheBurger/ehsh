all:
	cmake -S. -Bbuild
	cmake --build build

clean:
	cmake -E rm -rf build

test:
	echo TODO

cov:
	echo TODO

format:
	echo TODO

lint:
	echo TODO
