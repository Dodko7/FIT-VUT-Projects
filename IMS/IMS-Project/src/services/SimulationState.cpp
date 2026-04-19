#include "SimulationState.h"

SimulationState* SimulationState::instance = nullptr;

SimulationState::SimulationState() 
    : ConvoyCountTown1("Convoys from Town 1"),
      ConvoyCountTown4("Convoys from Town 4"),
      TotalDeliveredToTown5("Total delivered to Town 5 (tons)"),
      globalConvoyId(0),
      campaignCost(0.0),
      dailyDeliveryCounter(0.0),
      weatherDelayMultiplier(1.0),
      currentWeatherDelayPercentage(0.0),
      isWeatherActive(false),
      totalRobberAttacks(0),
      totalSuppliesStolen(0.0)
{
    dailyCosts.push_back(0.0);  // Day 0 starts with 0 cost
    town5DeliveriesHistory.push_back(0.0);  // Day 0 starts with 0 deliveries
    town5TotalDeliveredHistory.push_back(0.0);  // Day 0 starts with 0 total delivered
    weeklyCostsTown1.push_back(0.0);  // Week 0 starts with 0 cost for Town 1
    
    // Initialize weekly costs for all routes
    for (int i = 0; i < 4; i++) {
        weeklyCostsAllRoutes[i].push_back(0.0);
    }
    
    // Initialize RobberAttacksPerRoute array separately (can't use initializer list with Stat)
    RobberAttacksPerRoute[0].SetName("Robber attacks Town 1->2");
    RobberAttacksPerRoute[1].SetName("Robber attacks Town 2->3");
    RobberAttacksPerRoute[2].SetName("Robber attacks Town 3->4");
    RobberAttacksPerRoute[3].SetName("Robber attacks Town 4->5");
}

SimulationState* SimulationState::GetInstance() {
    if (instance == nullptr) {
        instance = new SimulationState();
    }
    return instance;
}

void SimulationState::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void SimulationState::Reset() {
    campaignCost = 0.0;
    globalConvoyId = 0;
    dailyDeliveryCounter = 0.0;
    town5SuppliesHistory.clear();
    town5DeliveriesHistory.clear();
    town5DeliveriesHistory.push_back(0.0);  // Day 0 starts with 0 deliveries
    town5TotalDeliveredHistory.clear();
    town5TotalDeliveredHistory.push_back(0.0);  // Day 0 starts with 0 total delivered
    campaignCostHistory.clear();
    dailyCosts.clear();
    dailyCosts.push_back(0.0);  // Day 0 starts with 0 cost
    weeklyCostsTown1.clear();
    weeklyCostsTown1.push_back(0.0);  // Week 0 starts with 0 cost for Town 1
    
    for (int i = 0; i < 4; i++) {
        weeklyCostsAllRoutes[i].clear();
        weeklyCostsAllRoutes[i].push_back(0.0);
    }
    
    weatherDelayMultiplier = 1.0;
    currentWeatherDelayPercentage = 0.0;
    isWeatherActive = false;
    totalRobberAttacks = 0;
    totalSuppliesStolen = 0.0;
    
    for (int i = 0; i < 4; i++) {
        RobberAttacksPerRoute[i].Clear();
    }
    
    ConvoyCountTown1.Clear();
    ConvoyCountTown4.Clear();
    TotalDeliveredToTown5.Clear();
}
