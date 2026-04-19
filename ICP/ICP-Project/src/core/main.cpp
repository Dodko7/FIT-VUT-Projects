#include "fsm.hpp"
#include "fsmErrors.hpp"
#include <QCoreApplication>  // Add this include
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {  // Add parameters to main
    // Create QCoreApplication instance first
    QCoreApplication app(argc, argv);

    try {
        // 1. Inicializácia a konfigurácia automatu TOF
        FSM fsm;
        fsm.setName("TOF");
        fsm.setDescription("Timer to off, supports setting timeout and reporting remaining time.");

        // Pridanie vstupov ako char
        fsm.addExpectedInput('i'); // Vstup 'in' (1=zapnúť, 0=vypnúť)
        fsm.addExpectedInput('s'); // Vstup 'set_to' (nastavenie timeoutu)
        fsm.addExpectedInput('r'); // Vstup 'req_rt' (požiadavka na zostávajúci čas)
        fsm.addExpectedInput('t'); // Input for timeout transitions

        // Pridanie premenných (používajú sa ako náhrada za výstupy, kým nie je implementované output())
        fsm.addVariable("out", "0");     // Výstup: 0=vypnuté, 1=zapnuté
        fsm.addVariable("rt", "0");      // Zostávajúci čas
        fsm.addVariable("timeout", "5000"); // Predvolený timeout (ms)
        fsm.addVariable("in_value", "0");     // Add a numeric input value variable

        // Pridanie stavov
        // Akcie sú uložené ako reťazce pre budúcu integráciu s QJSEngine
        // Výstupy sú statické char (kým nie je implementované output())
        fsm.addState("IDLE", "output('out', 0); output('rt', 0);", '0', true);
        fsm.addState("ACTIVE", "output('out', 1); output('rt', timeout);", '1', false);
        fsm.addState("TIMING", "output('rt', timeout - elapsed());", '0', false);

        // Nastavenie počiatočného stavu
        fsm.setStartState("IDLE");

        // Pridanie prechodov
        // Používame statické podmienky a vstupy, kým nie je implementované QJSEngine
        // Timeouty sú simulované statickým vstupom 't' (kým nie je implementovaný @timeout)
        fsm.addTransition("IDLE", "ACTIVE", "atoi(valueof('in_value')) == 1", 'i');
        fsm.addTransition("ACTIVE", "TIMING", "atoi(valueof('in_value')) == 0", 'i');
        fsm.addTransition("TIMING", "ACTIVE", "atoi(valueof('in_value')) == 1", 'i');
        fsm.addTransition("TIMING", "IDLE", "", 't'); // Simulácia timeoutu
        fsm.addTransition("IDLE", "IDLE", "", 's');   // Nastavenie timeoutu (zostáva v IDLE)
        fsm.addTransition("ACTIVE", "ACTIVE", "", 's'); // Nastavenie timeoutu
        fsm.addTransition("TIMING", "TIMING", "", 's'); // Nastavenie timeoutu
        fsm.addTransition("IDLE", "IDLE", "", 'r');   // Požiadavka na zostávajúci čas
        fsm.addTransition("ACTIVE", "ACTIVE", "", 'r'); // Požiadavka na zostávajúci čas
        fsm.addTransition("TIMING", "TIMING", "", 'r'); // Požiadavka na zostávajúci čas
        fsm.addTransition("ACTIVE", "IDLE", "", 't'); // Allow timeout from ACTIVE state

        // 2. Validácia FSM
        std::cout << "Validating initial FSM..." << std::endl;
        fsm.validateFSM();
        std::cout << "Initial FSM is valid" << std::endl;

        // 3. Uloženie FSM do JSON
        const std::string jsonFile = "examples/test_fsm.json";
        std::cout << "Saving FSM to " << jsonFile << "..." << std::endl;
        fsm.saveToJson(jsonFile);

        // Overenie vytvorenia JSON súboru
        std::ifstream checkJson(jsonFile);
        if (!checkJson.is_open()) {
            throw std::runtime_error("Failed to create JSON file: " + jsonFile);
        }
        checkJson.close();
        std::cout << "JSON file successfully created: " << jsonFile << std::endl;

        // 4. Načítanie FSM z JSON
        FSM loadedFsm;
        std::cout << "Loading FSM from " << jsonFile << "..." << std::endl;
        loadedFsm.loadFromJson(jsonFile);

        // Set the input before validation
        std::cout << "Setting input sequence: i t" << std::endl;
        loadedFsm.setInput("it"); // Set input first

        // Now validate the FSM with input
        std::cout << "Validating loaded FSM..." << std::endl;
        loadedFsm.validateFSM();
        std::cout << "Loaded FSM is valid" << std::endl;

        // Overenie zhody načítaného FSM s originálom
        if (loadedFsm.getName() != fsm.getName() ||
            loadedFsm.getDescription() != fsm.getDescription() ||
            loadedFsm.getStates().size() != fsm.getStates().size() ||
            loadedFsm.getStartState()->getName() != fsm.getStartState()->getName() ||
            loadedFsm.getExpectedInputs().size() != fsm.getExpectedInputs().size() ||
            loadedFsm.getVariables().size() != fsm.getVariables().size()) {
            throw std::runtime_error("Loaded FSM does not match original FSM");
        }
        std::cout << "Loaded FSM matches original FSM" << std::endl;

        // 5. Generovanie DOT súboru pre vizualizáciu
        std::cout << "Generating DOT file for visualization..." << std::endl;
        loadedFsm.debug();

        // Overenie vytvorenia DOT súboru
        const std::string dotFile = "assets/fsm_debug.dot";
        std::ifstream checkDot(dotFile);
        if (!checkDot.is_open()) {
            throw std::runtime_error("Failed to create DOT file: " + dotFile);
        }
        checkDot.close();
        std::cout << "DOT file successfully created: " << dotFile << std::endl;

        // Before running the FSM
        std::cout << "Verifying transitions in the loaded FSM..." << std::endl;
        for (const auto& statePair : loadedFsm.getStates()) {
            const auto& stateName = statePair.first;
            const auto& state = statePair.second;
            std::cout << "State: " << stateName << std::endl;
            
            // Display transitions from this state
            const auto& deps = state->getDependencies();
            for (const auto& dep : deps) {
                std::cout << "  Transition from: " << dep->getFromState()->getName() 
                          << " on input '" << dep->getInput() << "'"
                          << " with condition '" << dep->getCondition() << "'" << std::endl;
            }
        }

        // 6. Spustenie načítaného FSM s jednoduchou sekvenciou vstupov
        std::cout << "Setting in_value to 1 for the first 'i' input" << std::endl;
        loadedFsm.addVariable("in_value", "1", true);  // True for overwrite

        std::cout << "Running loaded FSM with input sequence: i t" << std::endl;
        loadedFsm.run();

        // Overenie výsledku behu
        if (loadedFsm.getCurrentState()->getName() == "TIMING" &&  // Change from IDLE to TIMING
            loadedFsm.getCurrentMachineState() == machineState::STOPPED) {
            std::cout << "FSM successfully reached TIMING state after input sequence" << std::endl;
        } else {
            throw std::runtime_error("FSM failed to reach expected state (TIMING)");
        }

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