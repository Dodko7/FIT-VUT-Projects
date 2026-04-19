all: cmake

cmake:
	@mkdir -p build
	@cd build && cmake .. && make -j12 

test:
	@cd build && make test

clean:
	@cd build && make clean

run:
	@cd build && ./ifj2024