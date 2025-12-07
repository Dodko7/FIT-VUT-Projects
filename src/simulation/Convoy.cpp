#include "Convoy.h"
#include "../services/SimulationState.h"
#include "../config/Config.h"
#include "../models/SupplyTypes.h"

Convoy::Convoy(int id, int from, int to, const TownSupplies& supplies) 
    : convoyId(id), fromTown(from), toTown(to), cargo(supplies) {}

void Convoy::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    // LOADING PHASE - Seize town, load cargo, release town
    Seize(*state->towns[fromTown]);
    
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        state->townStorage[fromTown].supplies[i] -= cargo.supplies[i];
    }
    state->townStorage[fromTown].wagons -= cargo.wagons;
    state->townStorage[fromTown].mules -= cargo.mules;
    
    Release(*state->towns[fromTown]);
    
    // TRAVEL PHASE
    double baseTravelTime = Uniform(Config::travelTimes[fromTown] * 0.9, Config::travelTimes[fromTown] * 1.1);
    double travelTime = baseTravelTime * state->weatherDelayMultiplier;
    double travelCost = Config::travelCosts[fromTown];
    state->campaignCost += travelCost;
    
    // Track costs by week for each route
    int currentWeek = (int)(Time / (24.0 * 7.0));  // Convert hours to weeks
    
    // Ensure the vector is large enough for this route
    while (state->weeklyCostsAllRoutes[fromTown].size() <= (size_t)currentWeek) {
        state->weeklyCostsAllRoutes[fromTown].push_back(0.0);
    }
    state->weeklyCostsAllRoutes[fromTown][currentWeek] += travelCost;
    
    // Track Town 1 costs by week (for separate Town 1 graph)
    if (fromTown == 0) {
        while (state->weeklyCostsTown1.size() <= (size_t)currentWeek) {
            state->weeklyCostsTown1.push_back(0.0);
        }
        state->weeklyCostsTown1[currentWeek] += travelCost;
    }
    
    Wait(travelTime);
    
    // ROBBER ATTACK CHECK (during travel)
    if (Config::runtime.robbersEnabled) {
        double attackChance = Random();
        if (attackChance < Config::robberProbability[fromTown]) {
            double stealPercentage = Uniform(Config::robberStealMin[fromTown], Config::robberStealMax[fromTown]);
            double cargoBeforeAttack = cargo.getTotal();
            
            // Steal percentage from each supply type
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                double stolen = cargo.supplies[i] * (stealPercentage / 100.0);
                cargo.supplies[i] -= stolen;
            }
            
            double cargoAfterAttack = cargo.getTotal();
            double actualStolen = cargoBeforeAttack - cargoAfterAttack;
            
            state->totalRobberAttacks++;
            state->totalSuppliesStolen += actualStolen;
            state->RobberAttacksPerRoute[fromTown](1);
        }
    }
    
    // ARRIVAL AND UNLOADING PHASE - Seize town, unload cargo, release town
    Seize(*state->towns[toTown]);
    
    state->townStorage[toTown].add(cargo);
    
    Release(*state->towns[toTown]);
    
    // Record statistics
    if (fromTown == 0) {
        state->ConvoyCountTown1(1);
    } else if (fromTown == 3) {
        state->TotalDeliveredToTown5(cargo.getTotal());
        state->dailyDeliveryCounter += cargo.getTotal();  // Track deliveries for the current day
        state->ConvoyCountTown4(1);
    }
    
    // CHAIN REACTION: Generate next convoy if not at Town 4 or Town 5
    if (toTown >= 1 && toTown <= 3) {
        state->globalConvoyId++;
        
        // Prepare cargo from current destination town
        TownSupplies nextCargo;
        
        double availableSupplies = state->townStorage[toTown].getTotal();
        
        if (availableSupplies > 0) {
            // Calculate wagons and mules needed
            int availableWagons = state->townStorage[toTown].wagons;
            int availableMules = state->townStorage[toTown].mules;
            
            // Use all available supplies for intermediate towns (2 and 3)
            double targetCargo = availableSupplies;
            
            int neededWagons = (int)(targetCargo / Config::WAGON_CAPACITY);
            if (neededWagons > availableWagons) neededWagons = availableWagons;
            
            double remainingCargo = targetCargo - (neededWagons * Config::WAGON_CAPACITY);
            int neededMules = (int)(remainingCargo / Config::MULE_CARRY_CAPACITY) + 1;
            if (neededMules > availableMules) neededMules = availableMules;
            
            double actualCapacity = neededWagons * Config::WAGON_CAPACITY + neededMules * Config::MULE_CARRY_CAPACITY;
            if (actualCapacity > targetCargo) actualCapacity = targetCargo;
            
            nextCargo.wagons = neededWagons;
            nextCargo.mules = neededMules;
            
            // Load supplies proportionally
            double ratio = actualCapacity / availableSupplies;
            if (ratio > 1.0) ratio = 1.0;
            
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                nextCargo.supplies[i] = state->townStorage[toTown].supplies[i] * ratio;
            }
            
            // Activate next convoy
            (new Convoy(state->globalConvoyId, toTown, toTown + 1, nextCargo))->Activate();
        }
    }
}
