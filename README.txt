# Moore Machine Debugger, Runner, and Editor
FIT BUT, 2025

## Overview
This project implements a Moore machine debugger, runner, and editor with a GUI using C++17 and Qt. The system allows users to create, edit, simulate, and debug finite state machines (FSMs), with a specific focus on Moore machines where outputs are associated with states rather than transitions.

## Features
- **Interactive GUI Interface**: Visual creation and editing of FSMs
- **State Management**: Add, edit, and remove states with associated outputs and actions
- **Transition Management**: Create and edit transitions between states with input conditions
- **JSON Serialization**: Save and load FSM definitions to/from JSON files
- **Simulation and Debugging**: Step-by-step execution, breakpoints, and real-time visualization
- **Script Support**: JavaScript-based action definitions for state behaviors
- **Validation**: Automatic detection of common FSM design issues (unreachable states, determinism violations)
- **Visualization**: Graphical representation of the FSM and its current state

## Components

### Core Library
The core FSM functionality is implemented in C++ and provides:
- Basic FSM structures and operations
- State and transition management
- Input/output processing
- JSON serialization/deserialization
- Validation and execution logic

### GUI
The graphical interface is built with Qt and provides:
- Drag-and-drop FSM editor
- State and transition inspectors/editors
- Real-time FSM visualization
- Debugging tools
- Input/output monitoring

## Directory Structure
- `src/`: Source code for the project
  - `core/`: Core FSM logic and implementation
  - `gui/`: Qt-based graphical user interface
- `tests/`: Unit tests for the core FSM functionality
- `examples/`: Example applications using the FSM library
- `lib/`: Third-party libraries (Catch2 for testing, nlohmann/json for JSON parsing)
- `docs/`: Project documentation
- `assets/`: Generated visualization files and resources

## Building the Project

### Prerequisites
- C++17 compatible compiler (clang++ or g++)
- Qt 5 or Qt 6 development libraries
- nlohmann/json library (included)
- Catch2 for tests (included)

### Using Make
```bash
# Build everything
make

# Build just the core library
make core_backend

# Build the examples
make examples

# Run the tests
make test
```

### Using CMake (for GUI)
```bash
# Navigate to the GUI directory
cd src/gui

# Create a build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make
```

## Running Examples
```bash
# Run the FSM interface example
./build/examples/fsm_interface_example
```

## Usage

### Core API
The core FSM functionality can be accessed through the `FSMManager` interface:

```cpp
#include "fsmInterface.hpp"

// Create an FSM manager
FSMManager manager;

// Create a new FSM
manager.createFSM("binary_counter", "Simple binary counter", std::chrono::milliseconds(100));

// Add states
manager.addState("s0", "", '0', false);
manager.addState("s1", "", '1', true);

// Add transitions
manager.addTransition("s0", "s1", "", '1');
manager.addTransition("s1", "s0", "", '0');

// Set the start state
manager.setStartState("s0");

// Set the input
manager.setInput("1010");

// Run the FSM
manager.run();

// Get the output
std::string output = manager.getOutput();
```

### GUI Application
The GUI application provides an interactive way to create and debug FSMs:

1. Launch the application from `src/gui/build/Automata_Editor`
2. Create a new FSM or load an existing one from a JSON file
3. Add states and transitions using the editor tools
4. Set the start state and any final states
5. Define inputs and run the simulation
6. Use step-by-step debugging to analyze the FSM behavior

## Authors
- xvalenk00 - Core FSM implementation, serialization
- xondre16 - Code execution on-state, variable management, serialization

## License
See the LICENSE file for details.

## Acknowledgments
This project was created as part of the ICP (The C++ Programming Language) course at FIT BUT.