#include <simlib.h>
#include <iostream>
#include <cstring>
#include "config/Config.h"
#include "services/SimulationState.h"
#include "services/ExperimentRunner.h"
#include "simulation/WeatherEvents.h"
#include "simulation/ConvoyGenerators.h"
#include "simulation/ConsumptionProcess.h"
#include "utils/Initialization.h"
#include "utils/Statistics.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help              Show this help message\n";
    std::cout << "  --weather           Enable weather events (default: on)\n";
    std::cout << "  --no-weather        Disable weather events\n";
    std::cout << "  --robbers           Enable robber attacks (default: on)\n";
    std::cout << "  --no-robbers        Disable robber attacks\n";
    std::cout << "  --experiment        Enable experiment mode (default: off)\n";
    std::cout << "  --days=N            Set simulation days (default: 100)\n";
    std::cout << "  --seed=N            Set random seed (default: 12345)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << "                    # Run with defaults (weather + robbers)\n";
    std::cout << "  " << programName << " --no-weather       # Run without weather events\n";
    std::cout << "  " << programName << " --no-robbers       # Run without robber attacks\n";
    std::cout << "  " << programName << " --days=150 --seed=42  # Custom days and seed\n";
    std::cout << "  " << programName << " --experiment       # Run in experiment mode\n";
}

void parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            exit(0);
        }
        else if (arg == "--weather") {
            Config::runtime.weatherEnabled = true;
        }
        else if (arg == "--no-weather") {
            Config::runtime.weatherEnabled = false;
        }
        else if (arg == "--robbers") {
            Config::runtime.robbersEnabled = true;
        }
        else if (arg == "--no-robbers") {
            Config::runtime.robbersEnabled = false;
        }
        else if (arg == "--experiment") {
            Config::runtime.experimentMode = true;
        }
        else if (arg.find("--days=") == 0) {
            Config::runtime.simulationDays = std::stoi(arg.substr(7));
        }
        else if (arg.find("--seed=") == 0) {
            Config::runtime.randomSeed = std::stoi(arg.substr(7));
        }
        else {
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cerr << "Use --help for usage information\n";
            exit(1);
        }
    }
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    parseArguments(argc, argv);
    
    // Print configuration
    std::cout << "\n==============================================\n";
    std::cout << "SIMULATION CONFIGURATION\n";
    std::cout << "==============================================\n";
    std::cout << "Simulation Days: " << Config::runtime.simulationDays << "\n";
    std::cout << "Random Seed: " << Config::runtime.randomSeed << "\n";
    std::cout << "Weather Events: " << (Config::runtime.weatherEnabled ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "Robber Attacks: " << (Config::runtime.robbersEnabled ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "Experiment Mode: " << (Config::runtime.experimentMode ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "==============================================\n\n";
    // Get singleton instance of simulation state
    auto* state = SimulationState::GetInstance();
    
    // Initialize facilities and town supplies
    Initialization::InitializeFacilities();
    Initialization::InitializeTownSupplies();
    
    if (Config::runtime.experimentMode) {
        // Run experiments if enabled
        ExperimentRunner::RunExperiments();
    }
    else {
        // Normal single simulation run
        Init(0, Config::runtime.simulationDays * 24.0);  // Convert days to hours
        RandomSeed(Config::runtime.randomSeed);
        
        // Schedule weather events if enabled
        if (Config::runtime.weatherEnabled) {
            (new BadWeatherEvent())->Activate(Time + Uniform(Config::WEATHER_INITIAL_DELAY_MIN, Config::WEATHER_INITIAL_DELAY_MAX));
        }
        
        // Activate convoy generators
        (new Town1ConvoyGenerator())->Activate();
        (new Town4ConvoyGenerator())->Activate();
        
        // Activate daily consumption process
        (new DailyConsumptionProcess())->Activate();
        
        // Run simulation
        Run();
        
        // Output final statistics
        Statistics::OutputFinalStatistics();
        if (Config::runtime.robbersEnabled) {
            Statistics::OutputRobberStatistics();
        }
    }
    
    return 0;
}
