#include "fsm.hpp"
#include "fsmErrors.hpp"
#include <iostream>
#include <fstream>

int main() {
    try {
        // 1. Initialize and configure the TOF FSM
        FSM fsm;
        fsm.setName("TOF");
        fsm.setDescription("Timer to off, supports setting timeout and reporting remaining time.");

        // Add inputs as strings
        fsm.addExpectedInput("in");
        fsm.addExpectedInput("set_to");
        fsm.addExpectedInput("req_rt");

        // Add variables (outputs are stored as variables until addOutput is implemented)
        fsm.addVariable("out", "0");
        fsm.addVariable("rt", "0");
        fsm.addVariable("timeout", "5000");

        // Add states with JavaScript actions
        fsm.addState("IDLE", "if (defined('set_to')) { timeout = atoi(valueof('set_to')); } output('out', '0'); output('rt', '0');", false, std::chrono::milliseconds(0));
        fsm.addState("ACTIVE", "if (defined('set_to')) { timeout = atoi(valueof('set_to')); } output('out', '1'); output('rt', timeout);", false);
        fsm.addState("TIMING", "if (defined('set_to')) { timeout = atoi(valueof('set_to')); } output('rt', timeout - elapsed());", false);

        // Add transitions ensuring determinism
        std::string idle = "IDLE", active = "ACTIVE", timing = "TIMING";
        fsm.addTransition(idle, active, "in", "atoi(valueof('in')) == 1", "", "");
        fsm.addTransition(active, timing, "in", "atoi(valueof('in')) == 0", "", "");
        fsm.addTransition(timing, active, "in", "atoi(valueof('in')) == 1", "", "");
        fsm.addTransition(timing, idle, "", "", "@ timeout", "");
        fsm.addTransition(idle, idle, "set_to", "", "", "");
        fsm.addTransition(active, active, "set_to", "", "", "");
        fsm.addTransition(timing, timing, "set_to", "", "", "");
        fsm.addTransition(idle, idle, "req_rt", "", "", "");
        fsm.addTransition(active, active, "req_rt", "", "", "");
        fsm.addTransition(timing, timing, "req_rt", "", "", "");

        // Set initial state
        fsm.setStartState("IDLE");

        // Validate FSM
        std::cout << "Validating initial FSM..." << std::endl;
        fsm.validateFSM();

        // 2. Save FSM to JSON
        const std::string jsonFile = "examples/test_fsm.json";
        std::cout << "Saving FSM to " << jsonFile << "..." << std::endl;
        fsm.saveToJson(jsonFile);

        // Verify JSON file creation
        std::ifstream checkJson(jsonFile);
        if (!checkJson.is_open()) {
            throw std::runtime_error("Failed to create JSON file: " + jsonFile);
        }
        checkJson.close();
        std::cout << "JSON file successfully created: " << jsonFile << std::endl;

        // 3. Load FSM from JSON
        FSM loadedFsm;
        std::cout << "Loading FSM from " << jsonFile << "..." << std::endl;
        loadedFsm.loadFromJson(jsonFile);

        // Validate loaded FSM
        std::cout << "Validating loaded FSM..." << std::endl;
        loadedFsm.validateFSM();

        // Verify loaded FSM matches original
        if (loadedFsm.getName() != fsm.getName() ||
            loadedFsm.getDescription() != fsm.getDescription() ||
            loadedFsm.getStates().size() != fsm.getStates().size() ||
            loadedFsm.getStartState()->getName() != fsm.getStartState()->getName()) {
            throw std::runtime_error("Loaded FSM does not match original FSM");
        }
        std::cout << "Loaded FSM matches original FSM" << std::endl;

        // 4. Generate DOT file for visualization
        std::cout << "Generating DOT file for visualization..." << std::endl;
        loadedFsm.debug();

        // Verify DOT file creation
        const std::string dotFile = "assets/fsm_debug.dot";
        std::ifstream checkDot(dotFile);
        if (!checkDot.is_open()) {
            throw std::runtime_error("Failed to create DOT file: " + dotFile);
        }
        checkDot.close();
        std::cout << "DOT file successfully created: " + dotFile << std::endl;

        // 5. Run loaded FSM with a simple input sequence
        std::cout << "Running loaded FSM with input sequence: in" << std::endl;
        loadedFsm.run("in");

    } catch (const FSMException& e) {
        std::cerr << "FSM Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    return 0;
}