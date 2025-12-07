#include "Statistics.h"
#include "../services/SimulationState.h"
#include "../services/GraphGenerator.h"
#include "../config/Config.h"
#include "../models/SupplyTypes.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void Statistics::OutputFinalStatistics() {
    auto* state = SimulationState::GetInstance();
    
    std::ofstream outputFile("output/simulation_output.txt");
    
    outputFile << "\n==============================================\n";
    outputFile << "SUPPLY CHAIN CAMPAIGN - FINAL REPORT\n";
    outputFile << "==============================================\n\n";
    
    outputFile << "SIMULATION PARAMETERS:\n";
    outputFile << "  Duration: " << Config::runtime.simulationDays << " days\n";
    outputFile << "  Weather System: " << (Config::runtime.weatherEnabled ? "Enabled" : "Disabled") << "\n";
    outputFile << "  Convoy Robbers: " << (Config::runtime.robbersEnabled ? "Enabled" : "Disabled") << "\n\n";
    
    outputFile << "FINAL TOWN INVENTORIES:\n";
    outputFile << "----------------------------------------------\n";
    for (int i = 0; i < Config::NUM_TOWNS; i++) {
        outputFile << "Town " << (i+1) << ":\n";
        outputFile << "  Grain:       " << std::fixed << std::setprecision(1) << state->townStorage[i].supplies[GRAIN] << " tons\n";
        outputFile << "  Fodder:      " << state->townStorage[i].supplies[FODDER] << " tons\n";
        outputFile << "  Salted Meat: " << state->townStorage[i].supplies[SALTED_MEAT] << " tons\n";
        outputFile << "  Wine/Oil:    " << state->townStorage[i].supplies[WINE_OIL] << " tons\n";
        outputFile << "  Equipment:   " << state->townStorage[i].supplies[EQUIPMENT] << " tons\n";
        outputFile << "  TOTAL:       " << state->townStorage[i].getTotal() << " tons\n";
        outputFile << "  Wagons:      " << state->townStorage[i].wagons << "\n";
        outputFile << "  Mules:       " << state->townStorage[i].mules << "\n\n";
    }
    
    outputFile << "CONVOY STATISTICS:\n";
    outputFile << "----------------------------------------------\n";
    outputFile << "Town 1 Convoys:\n";
    outputFile << "  Number: " << state->ConvoyCountTown1.Number() << "\n";
    outputFile << "  Mean: " << state->ConvoyCountTown1.MeanValue() << "\n";
    outputFile << "\nTown 4 Convoys:\n";
    outputFile << "  Number: " << state->ConvoyCountTown4.Number() << "\n";
    outputFile << "  Mean: " << state->ConvoyCountTown4.MeanValue() << "\n\n";
    
    outputFile << "FINANCIAL SUMMARY:\n";
    outputFile << "----------------------------------------------\n";
    outputFile << "Total Campaign Cost: " << std::fixed << std::setprecision(2) << state->campaignCost << " money units\n\n";
    
    if (Config::runtime.robbersEnabled) {
        outputFile << "ROBBER ATTACK SUMMARY:\n";
        outputFile << "----------------------------------------------\n";
        outputFile << "Total Robber Attacks: " << state->totalRobberAttacks << "\n";
        outputFile << "Attacks by route:\n";
        for (int route = 0; route < 4; route++) {
            outputFile << "  Route " << (route + 1) << ": " << (int)state->RobberAttacksPerRoute[route].Number() << " attacks\n";
        }
        outputFile << "\nTotal supplies stolen: " << std::fixed << std::setprecision(2) << state->totalSuppliesStolen << " tons\n\n";
    }
    
    outputFile << "==============================================\n";
    outputFile << "END OF REPORT\n";
    outputFile << "==============================================\n";
    
    outputFile.close();
}

void Statistics::OutputRobberStatistics() {
    auto* state = SimulationState::GetInstance();
    
    if (!Config::runtime.robbersEnabled) return;
}
