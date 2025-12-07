#include "Convoy.h"
#include "../services/SimulationState.h"
#include "../config/Config.h"
#include "../models/SupplyTypes.h"

Convoy::Convoy(int id, int from, int to, const TownSupplies& supplies) 
    : convoyId(id), fromTown(from), toTown(to), cargo(supplies) {}

void Convoy::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    Print("\n[TIME %.2f] Convoy #%d: Journey from Town %d to Town %d\n", 
          Time, convoyId, fromTown + 1, toTown + 1);
    
    // LOADING PHASE - Seize town, load cargo, release town
    Seize(*state->towns[fromTown]);
    
    Print("  Seizing Town %d for loading...\n", fromTown + 1);
    
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        state->townStorage[fromTown].supplies[i] -= cargo.supplies[i];
    }
    state->townStorage[fromTown].wagons -= cargo.wagons;
    state->townStorage[fromTown].mules -= cargo.mules;
    
    Print("  Loading: %.1f tons with %d wagons and %d mules\n", 
          cargo.getTotal(), cargo.wagons, cargo.mules);
    Print("    Cargo breakdown: G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f\n",
          cargo.supplies[GRAIN], cargo.supplies[FODDER],
          cargo.supplies[SALTED_MEAT], cargo.supplies[WINE_OIL], cargo.supplies[EQUIPMENT]);
    Print("  Town %d after loading: %.1f tons remaining\n", 
          fromTown + 1, state->townStorage[fromTown].getTotal());
    
    Release(*state->towns[fromTown]);
    Print("  Released Town %d\n", fromTown + 1);
    
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
    
    const char* weatherStatus = state->isWeatherActive ? " [BAD WEATHER]" : "";
    Print("  Traveling to Town %d...%s (Base: %.2f hours (%.1f days), Weather: %.2fx, Actual: %.2f hours (%.1f days))\n",
          toTown + 1, weatherStatus, baseTravelTime, baseTravelTime/24.0, 
          state->weatherDelayMultiplier, travelTime, travelTime/24.0);
    Print("  Cost: %.2f, Total campaign cost: %.2f\n", travelCost, state->campaignCost);
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
            
            Print("\n  *** ROBBER ATTACK! ***\n");
            Print("  Route: Town %d -> Town %d\n", fromTown + 1, toTown + 1);
            Print("  Stolen: %.1f%% (%.2f tons)\n", stealPercentage, actualStolen);
            Print("  Cargo remaining: %.2f tons\n\n", cargoAfterAttack);
        }
    }
    
    // ARRIVAL AND UNLOADING PHASE - Seize town, unload cargo, release town
    Print("[TIME %.2f] Convoy #%d: Arrived at Town %d\n", Time, convoyId, toTown + 1);
    
    Seize(*state->towns[toTown]);
    Print("  Seizing Town %d for unloading...\n", toTown + 1);
    
    Print("  Unloading %.1f tons at Town %d\n", cargo.getTotal(), toTown + 1);
    
    state->townStorage[toTown].add(cargo);
    
    Print("  Town %d now has: %.1f tons total (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n", 
          toTown + 1, state->townStorage[toTown].getTotal(),
          state->townStorage[toTown].supplies[GRAIN], state->townStorage[toTown].supplies[FODDER],
          state->townStorage[toTown].supplies[SALTED_MEAT], state->townStorage[toTown].supplies[WINE_OIL],
          state->townStorage[toTown].supplies[EQUIPMENT]);
    
    Release(*state->towns[toTown]);
    Print("  Released Town %d\n", toTown + 1);
    
    // Record statistics
    if (fromTown == 0) {
        state->ConvoyCountTown1(1);
    } else if (fromTown == 3) {
        state->TotalDeliveredToTown5(cargo.getTotal());
        state->ConvoyCountTown4(1);
    }
    
    // CHAIN REACTION: Generate next convoy if not at Town 4 or Town 5
    if (toTown >= 1 && toTown <= 3) {
        state->globalConvoyId++;
        Print("\n  >>> Generating next convoy #%d from Town %d to Town %d <<<\n", 
              state->globalConvoyId, toTown + 1, toTown + 2);
        
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
            
            Print("  Next convoy will carry %.1f tons with %d wagons and %d mules\n",
                  nextCargo.getTotal(), nextCargo.wagons, nextCargo.mules);
            
            // Activate next convoy
            (new Convoy(state->globalConvoyId, toTown, toTown + 1, nextCargo))->Activate();
        } else {
            Print("  No supplies available in Town %d - chain broken\n", toTown + 1);
        }
    }
    
    Print("  Convoy #%d process ending.\n", convoyId);
}
