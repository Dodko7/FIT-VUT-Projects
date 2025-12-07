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
            
            Print("\n[DAY %d] Town 5 consumed %.1f tons (%.1f tons remaining)\n", 
                  currentDay, consumptionToday, state->townStorage[4].getTotal());
        } else {
            Print("\n[DAY %d] Town 5 has NO SUPPLIES - starvation!\n", currentDay);
        }
        
        Release(*state->towns[4]);
        
        // Record supply and cost after consumption and update graphs
        state->town5SuppliesHistory.push_back(state->townStorage[4].getTotal());
        state->campaignCostHistory.push_back(state->campaignCost);
        GraphGenerator::GenerateSupplyGraph(currentDay);
        GraphGenerator::GenerateCostGraph(currentDay);
        
        // Track daily costs
        double previousCost = (currentDay > 0) ? state->campaignCostHistory[currentDay - 1] : 0.0;
        double todayCost = state->campaignCost - previousCost;
        state->dailyCosts.push_back(todayCost);
        GraphGenerator::GenerateDailyCostGraph(currentDay);
        GraphGenerator::GenerateTown1WeeklyCostGraph(currentDay);
        GraphGenerator::GenerateAllTownsWeeklyCostGraph(currentDay);
    }
}
