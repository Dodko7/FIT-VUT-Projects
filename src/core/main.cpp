#include "fsm.hpp"
#include "fsmErrors.hpp"
#include <iostream>
#include <fstream>

int main() {
    try {
        // 1. Inicializácia a konfigurácia automatu
        FSM fsm;
        fsm.setName("TOF");
        fsm.setDescription("Timer to off, simple version");
        fsm.addState("IDLE", "", false, std::chrono::milliseconds(0));  // With explicit stepDelay
        fsm.addState("TIMING", "", false);
        // fsm.addOutput("out", "0");
        std::string idle = "IDLE", timing = "TIMING";
        fsm.addTransition(idle, timing, "1", "", "", "");  
        fsm.addTransition(timing, idle, "0", "", "", ""); 
        fsm.setStartState("IDLE");

        // Validácia automatu
        std::cout << "Validating initial FSM..." << std::endl;
        fsm.validateFSM();

        // 2. Uloženie automatu do JSON
        const std::string jsonFile = "examples/test_fsm.json";
        std::cout << "Saving FSM to " << jsonFile << "..." << std::endl;
        fsm.saveToJson(jsonFile);

        // Overenie, či sa JSON súbor vytvoril
        std::ifstream checkJson(jsonFile);
        if (!checkJson.is_open()) {
            throw std::runtime_error("Failed to create JSON file: " + jsonFile);
        }
        checkJson.close();
        std::cout << "JSON file successfully created: " << jsonFile << std::endl;

        // 3. Načítanie automatu z JSON
        FSM loadedFsm;
        std::cout << "Loading FSM from " << jsonFile << "..." << std::endl;
        loadedFsm.loadFromJson(jsonFile);

        // Validácia načítaného automatu
        std::cout << "Validating loaded FSM..." << std::endl;
        loadedFsm.validateFSM();

        // Overenie, či načítaný automat zodpovedá pôvodnému
        if (loadedFsm.getName() != fsm.getName() ||
            loadedFsm.getDescription() != fsm.getDescription() ||
            loadedFsm.getStates().size() != fsm.getStates().size() ||
            loadedFsm.getStartState()->getName() != fsm.getStartState()->getName()) {
            throw std::runtime_error("Loaded FSM does not match original FSM");
        }
        std::cout << "Loaded FSM matches original FSM" << std::endl;

        // 4. Generovanie DOT súboru pre vizualizáciu
        std::cout << "Generating DOT file for visualization..." << std::endl;
        loadedFsm.debug();

        // Overenie, či sa DOT súbor vytvoril
        const std::string dotFile = "assets/fsm_debug.dot";
        std::ifstream checkDot(dotFile);
        if (!checkDot.is_open()) {
            throw std::runtime_error("Failed to create DOT file: " + dotFile);
        }
        checkDot.close();
        std::cout << "DOT file successfully created: " << dotFile << std::endl;

        // 5. Spustenie načítaného automatu (voliteľné, pre demonštráciu)
        std::cout << "Running loaded FSM with input sequence: 10" << std::endl;
        loadedFsm.run("10");

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