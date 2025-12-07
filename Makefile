# ============================================================================
# IMS Project Makefile
# ============================================================================
# This Makefile provides convenient commands for building and running
# your SIMLIB simulation models in Docker.
# ============================================================================

.PHONY: help build shell run clean clean-all test examples

# Default target - show help
help:
	@echo "IMS Project - Available Commands:"
	@echo "=================================="
	@echo ""
	@echo "  make build        - Build Docker container with SIMLIB"
	@echo "  make shell        - Start interactive shell in container"
	@echo "  make run          - Run all compiled programs"
	@echo ""
	@echo "  make examples     - Compile all examples"
	@echo "  make example/%    - Compile specific example (e.g., make example/simple_example)"
	@echo "  make run-example/% - Compile and run specific example"
	@echo ""
	@echo "  make main         - Compile all main project files"
	@echo "  make main/%       - Compile specific main file (e.g., make main/mymodel)"
	@echo "  make run-main/%   - Compile and run specific main file"
	@echo ""
	@echo "  make clean        - Remove compiled binaries"
	@echo "  make clean-all    - Remove binaries and output files"
	@echo "  make test         - Run all examples and check outputs"
	@echo ""

# Docker commands
# ============================================================================

build:
	@echo "Building Docker container..."
	cd docker && docker compose build

shell:
	@echo "Starting interactive shell..."
	cd docker && docker compose run --rm dev

# Compilation commands
# ============================================================================

# Compile all examples
examples:
	@echo "Compiling all examples..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p build/examples && \
		for file in src/examples/*.cc; do \
			name=$$(basename $$file .cc); \
			echo "Compiling $$name..."; \
			g++ -o build/examples/$$name $$file -lsimlib -lm || exit 1; \
		done'
	@echo "Examples compiled successfully!"

# Compile specific example
example/%:
	@echo "Compiling example: $*..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p build/examples && \
		g++ -o build/examples/$* src/examples/$*.cc -lsimlib -lm'
	@echo "Compiled: build/examples/$*"

# Compile and run specific example
run-example/%: example/%
	@echo "Running example: $*..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p output && \
		cd output && ../build/examples/$*'
	@echo "Output saved in output/"

# Compile all main project files (modular structure)
main:
	@echo "Compiling main project files..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p build/main && \
		echo "Compiling modular simulation project..."; \
		g++ -std=c++17 -I./simlib/src -I./src \
			src/models/SupplyTypes.cpp \
			src/models/TownSupplies.cpp \
			src/services/SimulationState.cpp \
			src/services/GraphGenerator.cpp \
			src/services/ExperimentRunner.cpp \
			src/simulation/WeatherEvents.cpp \
			src/simulation/Convoy.cpp \
			src/simulation/ConvoyGenerators.cpp \
			src/simulation/ConsumptionProcess.cpp \
			src/utils/Initialization.cpp \
			src/utils/Statistics.cpp \
			src/main/simulation.cpp \
			-L./simlib/src -lsimlib -lm -o build/main/simulation || exit 1; \
		echo "Build complete: build/main/simulation"; \
	'
	@echo "Main files compiled successfully!"

# Compile specific main file (with special handling for modular simulation)
main/%:
	@echo "Compiling main file: $*..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p build/main && \
		if [ "$*" = "simulation" ]; then \
			echo "Compiling modular simulation project..."; \
			g++ -std=c++17 -I./simlib/src -I./src \
				src/models/SupplyTypes.cpp \
				src/models/TownSupplies.cpp \
				src/services/SimulationState.cpp \
				src/services/GraphGenerator.cpp \
				src/services/ExperimentRunner.cpp \
				src/simulation/WeatherEvents.cpp \
				src/simulation/Convoy.cpp \
				src/simulation/ConvoyGenerators.cpp \
				src/simulation/ConsumptionProcess.cpp \
				src/utils/Initialization.cpp \
				src/utils/Statistics.cpp \
				src/main/simulation.cpp \
				-L./simlib/src -lsimlib -lm -o build/main/simulation; \
		elif [ -f "src/main/$*.cpp" ]; then \
			g++ -o build/main/$* src/main/$*.cpp -lsimlib -lm; \
		elif [ -f "src/main/$*.cc" ]; then \
			g++ -o build/main/$* src/main/$*.cc -lsimlib -lm; \
		else \
			echo "Error: src/main/$*.cpp or src/main/$*.cc not found"; \
			exit 1; \
		fi'
	@echo "Compiled: build/main/$*"

# Compile and run specific main file
run-main/%: main/%
	@echo "Running main file: $*..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p output && \
		cd output && ../build/main/$*'
	@echo "Output saved in output/"

# Utility commands
# ============================================================================

# Clean compiled binaries
clean:
	@echo "Cleaning compiled binaries..."
	rm -rf build/
	@echo "Clean complete!"

# Clean everything (binaries + outputs)
clean-all: clean
	@echo "Cleaning output files..."
	rm -rf output/*
	@echo "Clean all complete!"

# Test - compile and run all examples
test: examples
	@echo "Running all examples..."
	@cd docker && docker compose run --rm dev /bin/bash -c '\
		mkdir -p output/test && \
		for exe in build/examples/*; do \
			if [ -f "$$exe" ] && [ -x "$$exe" ]; then \
				name=$$(basename $$exe); \
				echo "Testing $$name..."; \
				cd output/test && ../../$$exe > $$name.log 2>&1 || echo "FAILED: $$name"; \
				cd ../..; \
			fi; \
		done'
	@echo "Test results in output/test/"

# Run compiled programs
run:
	@echo "Available programs:"
	@echo "Examples:"
	@cd docker && docker compose run --rm dev /bin/bash -c 'ls -1 build/examples/ 2>/dev/null || echo "  No examples compiled"'
	@echo ""
	@echo "Main:"
	@cd docker && docker compose run --rm dev /bin/bash -c 'ls -1 build/main/ 2>/dev/null || echo "  No main files compiled"'
	@echo ""
	@echo "Use 'make run-example/<name>' or 'make run-main/<name>' to run specific programs"

# Create necessary directories
init:
	@echo "Creating project directories..."
	@mkdir -p build/examples build/main output
	@echo "Directories created!"
