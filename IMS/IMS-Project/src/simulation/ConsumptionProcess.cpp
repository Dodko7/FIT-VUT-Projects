#include "ConsumptionProcess.h"
#include "../services/SimulationState.h"
#include "../services/GraphGenerator.h"
#include "../config/Config.h"
#include "../models/SupplyTypes.h"

void DailyConsumptionProcess::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    // Record initial supply and cost at day 0
    state->town5SuppliesHistory.push_back(state->townStorage[4].getTotal());
    state->campaignCostHistory.push_back(state->campaignCost);
    state->town5DeliveriesHistory[0] = 0.0;  // Day 0 has no deliveries yet
    state->town5TotalDeliveredHistory[0] = 0.0;  // Day 0 has no total delivered yet
    GraphGenerator::GenerateSupplyGraph(0);
    GraphGenerator::GenerateCostGraph(0);
    
    while (true) {
        Wait(Config::CONSUMPTION_INTERVAL);  // Wait 24 hours (1 day)
        
        int currentDay = (int)(Time / 24.0);  // Convert hours to days for display
        
        // Consume supplies from Town 5 (last town)
        Seize(*state->towns[4]);
        
        double availableSupplies = state->townStorage[4].getTotal();
        double consumptionToday = Config::CONSUMPTION_PER_DAY_ACTIVE;
        
        if (availableSupplies < consumptionToday) {
            consumptionToday = availableSupplies;
        }
        
        if (consumptionToday > 0) {
            // Consume supplies proportionally
            double ratio = consumptionToday / availableSupplies;
            
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                double consumed = state->townStorage[4].supplies[i] * ratio;
                state->townStorage[4].supplies[i] -= consumed;
            }
        }
        
        Release(*state->towns[4]);
        
        // Record deliveries for this day and reset counter for next day
        state->town5DeliveriesHistory.push_back(state->dailyDeliveryCounter);
        
        // Calculate and record total delivered (cumulative)
        double previousTotal = state->town5TotalDeliveredHistory.empty() ? 0.0 : state->town5TotalDeliveredHistory.back();
        state->town5TotalDeliveredHistory.push_back(previousTotal + state->dailyDeliveryCounter);
        
        state->dailyDeliveryCounter = 0.0;
        
        // Record supply and cost after consumption and update graphs
        state->town5SuppliesHistory.push_back(state->townStorage[4].getTotal());
        state->campaignCostHistory.push_back(state->campaignCost);
        GraphGenerator::GenerateSupplyGraph(currentDay);
        GraphGenerator::GenerateCostGraph(currentDay);
        GraphGenerator::GenerateTown5DeliveriesGraph(currentDay);
        
        // Track daily costs
        double previousCost = (currentDay > 0) ? state->campaignCostHistory[currentDay - 1] : 0.0;
        double todayCost = state->campaignCost - previousCost;
        state->dailyCosts.push_back(todayCost);
        GraphGenerator::GenerateDailyCostGraph(currentDay);
        GraphGenerator::GenerateAllTownsWeeklyCostGraph(currentDay);
    }
}
