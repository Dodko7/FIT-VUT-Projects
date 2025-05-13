/**
 * @file test_debug_helpers.hpp
 * @brief Debug context and utilities for test failures
 * 
 * This file provides utilities for capturing and formatting state information
 * during test execution. When a test fails, the debug context can provide
 * detailed information about the state of the system at the time of failure.
 * 
 * Key components:
 * - DebugContext class: Stores and formats named values for test debugging
 * - createFSMDebugContext: Helper to capture common FSM state for debugging
 * - TestUtils class: Provides fluent interface for building debug contexts
 * 
 * Usage example:
 * ```cpp
 * TestDebug::DebugContext context;
 * context.add("Value", 42);
 * context.addHeader("FSM State");
 * context.add("Current State", fsm.getCurrentState());
 * std::string debugInfo = context.toString();
 * ```
 * 
 * @author xvalenk00
 * @date May 2025
 * @version 1.0
 */

#ifndef TEST_DEBUG_HELPERS_HPP
#define TEST_DEBUG_HELPERS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <iomanip>
#include "fsmInterface.hpp"  // Include FSMManager definition

/**
 * @file test_debug_helpers.hpp
 * @brief Utility functions for debugging test failures
 * 
 * This file provides helper functions for debugging test failures by
 * capturing and formatting state information.
 */

namespace TestDebug {

/**
 * @brief Container for a named debug value
 */
struct DebugValue {
    std::string name;
    std::string value;
    
    DebugValue(const std::string& n, const std::string& v) : name(n), value(v) {}
};

/**
 * @brief Debug context container that collects state information
 */
class DebugContext {
private:
    struct Section {
        std::string name;
        std::unordered_map<std::string, std::string> values;
    };
    
    std::vector<Section> sections;

public:
    /**
     * @brief Add a header for a new section
     * @param name Name of the section
     */
    void addHeader(const std::string& name) {
        sections.push_back({name, {}});
    }
    
    /**
     * @brief Add a key-value pair to the current section
     * @param key Name of the entry
     * @param value Value to store, automatically converted to string
     */
    template<typename T>
    void add(const std::string& key, const T& value) {
        if (sections.empty()) {
            // Create a default section if none exists
            addHeader("Debug Information");
        }
        sections.back().values[key] = std::to_string(value);
    }
    
    /**
     * @brief Specialization for string values
     */
    void add(const std::string& key, const std::string& value) {
        if (sections.empty()) {
            addHeader("Debug Information");
        }
        sections.back().values[key] = value;
    }
    
    /**
     * @brief Specialization for const char* values
     */
    void add(const std::string& key, const char* value) {
        if (sections.empty()) {
            addHeader("Debug Information");
        }
        sections.back().values[key] = value;
    }
    
    /**
     * @brief Specialization for boolean values (for readability)
     */
    void add(const std::string& key, bool value) {
        if (sections.empty()) {
            addHeader("Debug Information");
        }
        sections.back().values[key] = value ? "1" : "0";
    }
    
    /**
     * @brief Specialization for vectors
     */
    template<typename T>
    void add(const std::string& key, const std::vector<T>& values) {
        if (sections.empty()) {
            addHeader("Debug Information");
        }
        
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < values.size(); ++i) {
            ss << values[i];
            if (i < values.size() - 1) {
                ss << ", ";
            }
        }
        ss << "]";
        
        sections.back().values[key] = ss.str();
    }
    
    /**
     * @brief Specialization for maps
     */
    template<typename K, typename V>
    void add(const std::string& key, const std::map<K, V>& values) {
        if (sections.empty()) {
            addHeader("Debug Information");
        }
        
        std::stringstream ss;
        ss << "{";
        size_t i = 0;
        for (const auto& [k, v] : values) {
            ss << k << ": " << v;
            if (++i < values.size()) {
                ss << ", ";
            }
        }
        ss << "}";
        
        sections.back().values[key] = ss.str();
    }
    
    /**
     * @brief Specialization for unordered_maps
     */
    template<typename K, typename V>
    void add(const std::string& key, const std::unordered_map<K, V>& values) {
        if (sections.empty()) {
            addHeader("Debug Information");
        }
        
        std::stringstream ss;
        ss << "{";
        size_t i = 0, size = values.size();
        for (const auto& [k, v] : values) {
            ss << k << ": " << v;
            if (++i < size) {
                ss << ", ";
            }
        }
        ss << "}";
        
        sections.back().values[key] = ss.str();
    }
    
    /**
     * @brief Get a formatted string of all debug values
     */
    std::string toString() const {
        std::stringstream result;
        
        result << "--- Debug Context ---\n\n";
        
        for (const auto& section : sections) {
            result << "## " << section.name << " ##\n";
            result << "----------------------------------------\n";
            
            for (const auto& [key, value] : section.values) {
                result << key << ": " << value << "\n";
            }
            
            if (&section != &sections.back()) {
                result << "\n";
            }
        }
        
        result << "----------------------------------------\n";
        return result.str();
    }
    
    /**
     * @brief Parse debug context from a string
     * @param str String containing debug context information
     */
    void parseFromString(const std::string& str) {
        sections.clear();
        
        std::istringstream stream(str);
        std::string line;
        Section* currentSection = nullptr;
        
        while (std::getline(stream, line)) {
            // Skip the "--- Debug Context ---" line
            if (line.find("--- Debug Context ---") != std::string::npos) {
                continue;
            }
            
            // Skip empty lines
            if (line.empty()) {
                continue;
            }
            
            // Check for section headers
            if (line.find("##") != std::string::npos) {
                std::string header = line;
                size_t start = line.find("##") + 2;
                size_t end = line.rfind("##");
                
                if (end != std::string::npos && end > start) {
                    header = line.substr(start, end - start);
                } else {
                    header = line.substr(start);
                }
                
                // Trim whitespace
                header.erase(0, header.find_first_not_of(" \t"));
                header.erase(header.find_last_not_of(" \t") + 1);
                
                sections.push_back({header, {}});
                currentSection = &sections.back();
                continue;
            }
            
            // Skip separator lines
            if (line.find("----") != std::string::npos) {
                continue;
            }
            
            // Parse key-value pairs
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos && currentSection != nullptr) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                currentSection->values[key] = value;
            }
        }
    }
};

/**
 * @brief Create a debug context to capture state information
 * @return A new DebugContext object
 */
inline DebugContext createContext() {
    return DebugContext();
}

/**
 * @brief Specialized FSM debug context builder
 * 
 * Helper function to create a debug context with common FSM state information.
 * This makes it easier to consistently capture the relevant state for FSM tests.
 * 
 * @param manager The FSM manager to get state information from
 * @return A DebugContext object populated with FSM state information
 */
inline DebugContext createFSMDebugContext(const FSMManager& manager) {
    DebugContext context;
    context.addHeader("FSM State");
    
    // Current state
    std::string currentState = "(uninitialized)";
    try {
        currentState = manager.getCurrentState();
    } catch (...) {
        currentState = "(error getting current state)";
    }
    context.add("Current State", currentState);
    
    // Machine state as string - avoid using enum directly to prevent type issues
    std::string machineStateStr = "(unknown)";
    try {
        // Use a string representation to avoid enum type issues
        // Let the caller handle specific enum conversions if needed
        auto stateVal = static_cast<int>(manager.getMachineState());
        switch (stateVal) {
            case 0: machineStateStr = "IDLE"; break;
            case 1: machineStateStr = "RUNNING"; break;
            case 2: machineStateStr = "PAUSED"; break;
            case 3: machineStateStr = "ERROR"; break;
            case 4: machineStateStr = "STOPPED"; break;
            default: machineStateStr = "UNKNOWN"; break;
        }
    } catch (...) {
        machineStateStr = "(error getting machine state)";
    }
    context.add("Machine State", machineStateStr);
    
    // Get states
    try {
        auto stateNames = manager.getAllStateNames();
        context.add("State Count", stateNames.size());
        context.add("States", stateNames);
    } catch (...) {
        context.add("Error", "Failed to get state names");
    }
    
    // Start state - note we don't have direct access to start state name
    try {
        // FSMManager doesn't have getStartState() so we can't directly access it
        // We'll just note that we can't retrieve it
        context.add("Start State", "(Not directly accessible)");
    } catch (...) {
        context.add("Start State", "(error getting start state)");
    }
    
    // Input string if any
    try {
        std::string input = manager.getInput();
        context.add("Input String", input.empty() ? "(empty)" : input);
    } catch (...) {
        context.add("Input String", "(error getting input string)");
    }
    
    // Output string if any
    try {
        std::string output = manager.getOutput();
        context.add("Output", output.empty() ? "(empty)" : output);
    } catch (...) {
        context.add("Output", "(error getting output)");
    }
    
    return context;
}

/**
 * @brief Common test utility class to wrap debugging and output functions
 */
class TestUtils {
private:
    DebugContext debugContext;
    
public:
    TestUtils() : debugContext() {}
    
    /**
     * @brief Add structured debug info about FSM state
     */
    TestUtils& captureState(const FSMManager& manager) {
        // Create FSM debug context
        DebugContext fsmContext = createFSMDebugContext(manager);
        
        // Copy all sections and values from fsmContext to debugContext
        // Note: We don't have getValues() or addValue() methods, 
        // so we need to manually copy the content using public methods
        
        // Get the formatted string from fsmContext and parse it
        std::string fsmData = fsmContext.toString();
        this->debugContext.parseFromString(fsmData);
        
        return *this;
    }
    
    /**
     * @brief Add a section header
     */
    TestUtils& addSection(const std::string& title) {
        debugContext.addHeader(title);
        return *this;
    }
    
    /**
     * @brief Add a debug value
     */
    template<typename T>
    TestUtils& add(const std::string& name, const T& value) {
        debugContext.add(name, value);
        return *this;
    }
    
    /**
     * @brief Get the debug context (instead of executing assertions directly)
     * This prevents circular dependency with TestFramework
     */
    const DebugContext& getContext() const {
        return debugContext;
    }
};

/**
 * @brief Create a test utility object
 */
inline TestUtils createTestUtils() {
    return TestUtils();
}

} // namespace TestDebug

#endif // TEST_DEBUG_HELPERS_HPP
