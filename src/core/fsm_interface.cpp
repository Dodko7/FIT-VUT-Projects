#include "fsm_interface.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

FSMManager::FSMManager() : 
    currentFsm(nullptr), 
    lastError(""),
    previousState("") {
    std::cout << "FSMManager constructor called" << std::endl;
}

bool FSMManager::createFSM(const std::string& name, const std::string& description, const std::chrono::milliseconds stepDelay) {
    try {
        currentFsm = std::make_shared<FSM>();
        currentFsm->setName(name);
        currentFsm->setDescription(description);
        currentFsm->setStepDelay(stepDelay);
        currentFsm->setCurrentMachineState(machineState::IDLE);
        std::cout << "FSM created: " << name << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

bool FSMManager::loadFSM(const std::string& filename) {
    std::cout << "loadFSM called with filename: " << filename << std::endl;
    try {
        currentFsm = std::make_shared<FSM>();
        currentFsm->loadFromJson(filename);
        std::cout << "FSM loaded from: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "loadFSM error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::saveFSM(const std::string& filename) {
    std::cout << "saveFSM called with filename: " << filename << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "saveFSM error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->saveToJson(filename);
        std::cout << "FSM saved to: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "saveFSM error: " << lastError << std::endl;
        return false;
    }
}

std::string FSMManager::exportFSMToJson() {
    std::cout << "exportFSMToJson called" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "exportFSMToJson error: " << lastError << std::endl;
        return "";
    }

    try {
        // Create a temporary filename
        std::string tempFile = "temp_fsm_export.json";
        
        // Save to temporary file
        currentFsm->saveToJson(tempFile);
        
        // Read the file content
        std::ifstream inFile(tempFile);
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        inFile.close();
        
        // Remove the temporary file
        std::remove(tempFile.c_str());
        
        std::cout << "FSM exported to JSON successfully" << std::endl;
        return buffer.str();
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "exportFSMToJson error: " << lastError << std::endl;
        return "";
    }
}

bool FSMManager::importFSMFromJson(const std::string& jsonString) {
    std::cout << "importFSMFromJson called" << std::endl;
    try {
        // Create a temporary file
        std::string tempFile = "temp_fsm_import.json";
        
        // Write the JSON string to the file
        std::ofstream outFile(tempFile);
        outFile << jsonString;
        outFile.close();
        
        // Load the FSM from the file
        currentFsm = std::make_shared<FSM>();
        currentFsm->loadFromJson(tempFile);
        
        // Remove the temporary file
        std::remove(tempFile.c_str());
        
        std::cout << "FSM imported from JSON successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "importFSMFromJson error: " << lastError << std::endl;
        return false;
    }
}

std::string FSMManager::getLastError() const {
    std::cout << "getLastError called, returning: " << lastError << std::endl;
    return lastError;
}

// State Management

bool FSMManager::addState(const std::string& name, const std::string& action, char output, 
                         bool isFinal, std::chrono::milliseconds stepDelay) {
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        return false;
    }

    try {
        currentFsm->addState(name, action, output, isFinal, stepDelay);
        std::cout << "State added: " << name << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

bool FSMManager::removeState(const std::string& name) {
    std::cout << "removeState called for: " << name << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "removeState error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->removeState(name);
        std::cout << "State removed: " << name << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "removeState error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::setStartState(const std::string& name) {
    std::cout << "setStartState called with name: " << name << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "setStartState error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->setStartState(name);
        std::cout << "Start state set to: " << name << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "setStartState error: " << lastError << std::endl;
        return false;
    }
}

nlohmann::json FSMManager::getStateInfo(const std::string& name) {
    std::cout << "getStateInfo called for state: " << name << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "getStateInfo error: " << lastError << std::endl;
        return nlohmann::json();
    }

    auto state = currentFsm->getStatePtrByName(name);
    if (!state) {
        lastError = "State not found: " + name;
        std::cout << "getStateInfo error: " << lastError << std::endl;
        return nlohmann::json();
    }

    // Create a JSON object with state information
    nlohmann::json stateInfo;
    stateInfo["name"] = state->getName();
    stateInfo["action"] = state->getAction();
    stateInfo["output"] = state->getOutput();
    stateInfo["isFinal"] = state->getIsFinal();
    stateInfo["stepDelay"] = state->getStepDelay().count();
    
    // Get next states
    nlohmann::json nextStates = nlohmann::json::array();
    for (const auto& nextState : state->getNextStates()) {
        nextStates.push_back(nextState->getName());
    }
    stateInfo["nextStates"] = nextStates;
    
    // Get dependencies (transitions)
    nlohmann::json dependencies = nlohmann::json::array();
    for (auto& dep : state->getDependencies()) {
        nlohmann::json depInfo;
        depInfo["input"] = dep->getInput();
        depInfo["condition"] = dep->getCondition();
        
        auto fromState = dep->getFromState();
        if (fromState) {
            depInfo["fromState"] = fromState->getName();
        } else {
            depInfo["fromState"] = nullptr;
        }
        
        dependencies.push_back(depInfo);
    }
    stateInfo["dependencies"] = dependencies;
    
    return stateInfo;
}

std::vector<std::string> FSMManager::getAllStateNames() const {
    std::cout << "getAllStateNames called" << std::endl;
    if (!currentFsm) {
        std::cout << "getAllStateNames: No FSM is currently loaded" << std::endl;
        return {};
    }
    
    auto names = currentFsm->getAllStateNames();
    std::cout << "Returning " << names.size() << " state names" << std::endl;
    return names;
}

bool FSMManager::updateState(const std::string& name, const std::string& newName, 
                            const std::string& action, char output,
                            bool isFinal, bool updateIsFinal,
                            std::chrono::milliseconds stepDelay, bool updateStepDelay) {
    std::cout << "updateState called for: " << name << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "updateState error: " << lastError << std::endl;
        return false;
    }

    auto state = currentFsm->getStatePtrByName(name);
    if (!state) {
        lastError = "State not found: " + name;
        std::cout << "updateState error: " << lastError << std::endl;
        return false;
    }

    try {
        // Update state properties
        if (!newName.empty() && newName != name) {
            state->setName(newName);
        }
        
        if (!action.empty()) {
            state->setAction(action);
        }
        
        if (output != '\0') {
            state->setOutput(output);
        }
        
        if (updateIsFinal) {
            state->setIsFinal(isFinal);
        }
        
        if (updateStepDelay) {
            state->setStepDelay(stepDelay);
        }
        
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

// Transition Management

bool FSMManager::addTransition(const std::string& fromState, const std::string& toState, 
                              const std::string& condition, char input) {
    std::cout << "addTransition called from: " << fromState << " to: " << toState 
              << " with input: '" << input << "'" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "addTransition error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->addTransition(fromState, toState, condition, input);
        std::cout << "Transition added successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "addTransition error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::removeTransition(const std::string& fromState, const std::string& toState, char input) {
    std::cout << "removeTransition called from: " << fromState << " to: " << toState 
              << " with input: '" << input << "'" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "removeTransition error: " << lastError << std::endl;
        return false;
    }

    try {
        std::string fromStateCopy = fromState;
        std::string toStateCopy = toState;
        currentFsm->removeTransition(fromStateCopy, toStateCopy, input);
        std::cout << "Transition removed successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "removeTransition error: " << lastError << std::endl;
        return false;
    }
}

nlohmann::json FSMManager::getStateTransitions(const std::string& stateName) {
    std::cout << "getStateTransitions called for state: " << stateName << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "getStateTransitions error: " << lastError << std::endl;
        return nlohmann::json::array();
    }

    auto state = currentFsm->getStatePtrByName(stateName);
    if (!state) {
        lastError = "State not found: " + stateName;
        std::cout << "getStateTransitions error: " << lastError << std::endl;
        return nlohmann::json::array();
    }

    nlohmann::json transitions = nlohmann::json::array();
    
    // Get all transitions
    for (auto& dep : state->getDependencies()) {
        nlohmann::json transition;
        transition["input"] = std::string(1, dep->getInput());
        transition["condition"] = dep->getCondition();
        
        auto fromState = dep->getFromState();
        if (fromState) {
            transition["fromState"] = fromState->getName();
            transition["toState"] = stateName; // This is the destination state
        } else {
            transition["fromState"] = nullptr;
            transition["toState"] = stateName; // This is the destination state
        }
        
        transitions.push_back(transition);
    }
    
    return transitions;
}

bool FSMManager::updateTransition(const std::string& fromState, const std::string& toState, 
                                 char oldInput, char newInput, const std::string& newCondition) {
    std::cout << "updateTransition called from: " << fromState << " to: " << toState 
              << " oldInput: '" << oldInput << "' newInput: '" << newInput << "'" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "updateTransition error: " << lastError << std::endl;
        return false;
    }

    try {
        // First remove the old transition
        std::string fromStateCopy = fromState;
        std::string toStateCopy = toState;
        currentFsm->removeTransition(fromStateCopy, toStateCopy, oldInput);
        
        // Then add the new transition
        currentFsm->addTransition(fromState, toState, newCondition, newInput);
        
        std::cout << "Transition updated successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "updateTransition error: " << lastError << std::endl;
        return false;
    }
}

// Input and Variable Management

bool FSMManager::addExpectedInput(char input) {
    std::cout << "addExpectedInput called with input: '" << input << "'" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "addExpectedInput error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->addExpectedInput(input);
        std::cout << "Expected input added: '" << input << "'" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "addExpectedInput error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::removeExpectedInput(char input) {
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        return false;
    }

    try {
        currentFsm->removeExpectedInput(input);
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

std::vector<char> FSMManager::getExpectedInputs() const {
    if (!currentFsm) {
        return {};
    }
    
    std::vector<char> result;
    for (char input : currentFsm->getExpectedInputs()) {
        result.push_back(input);
    }
    
    return result;
}

bool FSMManager::setInput(const std::string& input) {
    std::cout << "setInput() called with: '" << input << "'" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "setInput() error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->setInput(input);
        std::cout << "Input set successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "setInput() error: " << lastError << std::endl;
        return false;
    }
}

std::string FSMManager::getInput() const {
    if (!currentFsm) {
        return "";
    }
    
    return currentFsm->getInput();
}

std::string FSMManager::getOutput() const {
    std::cout << "getOutput() called" << std::endl;
    if (!currentFsm) {
        std::cout << "getOutput(): No FSM is currently loaded" << std::endl;
        return "";
    }
    
    std::string output = currentFsm->getOutput();
    std::cout << "Current output: " << output << std::endl;
    return output;
}

bool FSMManager::addVariable(const std::string& name, const std::string& value, bool overwrite) {
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        return false;
    }

    try {
        currentFsm->addVariable(name, value, overwrite);
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

bool FSMManager::removeVariable(const std::string& name) {
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        return false;
    }

    try {
        currentFsm->removeVariable(name);
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

std::unordered_map<std::string, std::string> FSMManager::getVariables() const {
    if (!currentFsm) {
        return {};
    }
    
    return currentFsm->getVariables();
}

// Execution Control

bool FSMManager::run() {
    std::cout << "run() called" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "run() error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->run();
        std::cout << "FSM run completed successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "run() error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::step() {
    std::cout << "step() called" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "step() error: " << lastError << std::endl;
        return false;
    }

    try {
        bool result = currentFsm->debugStep();
        if (result) {
            std::cout << "Step successful, current state: " << currentFsm->getCurrentState()->getName() << std::endl;
        } else {
            std::cout << "Step ended FSM execution or reached final state" << std::endl;
        }
        return result;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "step() error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::reset() {
    std::cout << "reset() called" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "reset() error: " << lastError << std::endl;
        return false;
    }

    try {
        // Reset current state to start state
        currentFsm->setCurrentState(currentFsm->getStartState());
        // Clear output
        currentFsm->clearOutput();
        // Set machine state to IDLE
        currentFsm->setCurrentMachineState(machineState::IDLE);
        std::cout << "FSM reset successful, current state: " << currentFsm->getStartState()->getName() << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "reset() error: " << lastError << std::endl;
        return false;
    }
}

std::string FSMManager::getCurrentState() const {
    std::cout << "getCurrentState() called" << std::endl;
    if (!currentFsm || !currentFsm->getCurrentState()) {
        std::cout << "getCurrentState(): No current state available" << std::endl;
        return "";
    }
    
    std::string stateName = currentFsm->getCurrentState()->getName();
    std::cout << "Current state: " << stateName << std::endl;
    return stateName;
}

machineState FSMManager::getMachineState() const {
    if (!currentFsm) {
        return machineState::ERROR;
    }
    
    return currentFsm->getCurrentMachineState();
}

bool FSMManager::setMachineState(machineState state) {
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        return false;
    }

    try {
        currentFsm->setCurrentMachineState(state);
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

bool FSMManager::validateFSM() {
    std::cout << "validateFSM() called" << std::endl;
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        std::cout << "validateFSM() error: " << lastError << std::endl;
        return false;
    }

    try {
        currentFsm->validateFSM();
        std::cout << "FSM validation successful!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        std::cout << "validateFSM() error: " << lastError << std::endl;
        return false;
    }
}

bool FSMManager::pruneUnreachableStates() {
    if (!currentFsm) {
        lastError = "No FSM is currently loaded";
        return false;
    }

    try {
        currentFsm->pruneUnreachable();
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

std::shared_ptr<FSM> FSMManager::getFSM() const {
    return currentFsm;
}