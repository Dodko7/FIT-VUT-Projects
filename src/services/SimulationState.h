#ifndef SIMULATION_STATE_H
#define SIMULATION_STATE_H

#include <simlib.h>
#include <vector>
#include <fstream>
#include "../models/TownSupplies.h"
#include "../models/ExperimentResult.h"
#include "../config/Config.h"

class SimulationState {
private:
    static SimulationState* instance;
    SimulationState();
    
public:
    // Singleton access
    static SimulationState* GetInstance();
    static void DestroyInstance();
    
    // Town facilities and storage
    std::vector<Facility*> towns;
    TownSupplies townStorage[5];  // Using constant size to match Config::NUM_TOWNS
    
    // Statistics
    Stat ConvoyCountTown1;
    Stat ConvoyCountTown4;
    Stat TotalDeliveredToTown5;
    
    // Global convoy ID
    int globalConvoyId;
    
    // Graph data
    std::vector<double> town5SuppliesHistory;
    std::vector<double> town5DeliveriesHistory;  // Track daily deliveries to Town 5
    std::vector<double> town5TotalDeliveredHistory;  // Track cumulative deliveries to Town 5
    double dailyDeliveryCounter;  // Accumulator for deliveries within current day
    std::ofstream graphFile;
    
    // Economics tracking
    double campaignCost;
    std::vector<double> campaignCostHistory;
    std::ofstream costGraphFile;
    
    // Daily cost tracking
    std::vector<double> dailyCosts;
    std::vector<double> weeklyCostsTown1;  // Track Town 1 weekly costs
    std::vector<double> weeklyCostsAllRoutes[4];  // Track weekly costs for each route (Town 1->2, 2->3, 3->4, 4->5)
    
    // Weather system
    double weatherDelayMultiplier;
    double currentWeatherDelayPercentage;
    bool isWeatherActive;
    
    // Robber system tracking
    int totalRobberAttacks;
    double totalSuppliesStolen;
    Stat RobberAttacksPerRoute[4];
    
    // Experiment mode data
    std::vector<ExperimentResult> experimentResults;
    
    // Reset function for experiments
    void Reset();
};

#endif // SIMULATION_STATE_H
