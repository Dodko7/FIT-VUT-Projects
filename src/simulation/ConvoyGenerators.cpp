#include "ConvoyGenerators.h"
#include "Convoy.h"
#include "../services/SimulationState.h"
#include "../config/Config.h"
#include "../models/SupplyTypes.h"

void Town1ConvoyGenerator::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    state->globalConvoyId++;
    Print("\n========== GENERATING CONVOY #%d FROM TOWN 1 TO TOWN 2 ==========\n", state->globalConvoyId);
    
    // Prepare cargo from Town 1
    TownSupplies cargo;
    
    // Fixed composition: 100 wagons + 200 mules
    int wagons = Config::WAGONS_PER_CONVOY_T1;
    int mules = Config::MULES_PER_CONVOY_T1;
    
    double wagonCapacity = wagons * Config::WAGON_CAPACITY;
    double muleCapacity = mules * Config::MULE_CARRY_CAPACITY;
    double totalCapacity = wagonCapacity + muleCapacity;
    
    Print("  Preparing convoy: %d wagons (%.1f tons) + %d mules (%.1f tons) = %.1f tons capacity\n",
          wagons, wagonCapacity, mules, muleCapacity, totalCapacity);
    
    // Load supplies with specified ranges
    cargo.supplies[GRAIN] = Uniform(40, 50);
    cargo.supplies[FODDER] = Uniform(10, 15);
    cargo.supplies[SALTED_MEAT] = Uniform(3, 6);
    cargo.supplies[WINE_OIL] = Uniform(3, 6);
    cargo.supplies[EQUIPMENT] = Uniform(0.5, 2);
    cargo.wagons = wagons;
    cargo.mules = mules;
    
    Print("  Cargo prepared: %.1f tons (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n",
          cargo.getTotal(), cargo.supplies[GRAIN], cargo.supplies[FODDER], 
          cargo.supplies[SALTED_MEAT], cargo.supplies[WINE_OIL], cargo.supplies[EQUIPMENT]);
    
    // Start convoy from Town 1 to Town 2
    (new Convoy(state->globalConvoyId, 0, 1, cargo))->Activate();
    
    // Dynamic convoy scheduling based on Town 5 supply levels
    double town5Supplies = state->townStorage[4].getTotal();
    double nextInterval;
    
    if (town5Supplies < 500.0) {
        // Critical supply level - increase frequency (5 convoys per week)
        nextInterval = (7.0 * 24.0) / 5.0;  // ~33.6 hours
        Print("  [URGENT] Town 5 supplies LOW (%.1f tons) - Increasing convoy frequency to 5/week (next in %.1f hours)\n", 
              town5Supplies, nextInterval);
    } else {
        // Normal supply level - standard frequency (3 convoys per week)
        nextInterval = Config::CONVOY_INTERVAL_T1;  // ~56 hours
    }
    
    // Schedule next convoy
    Activate(Time + nextInterval);
}

void Town4ConvoyGenerator::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    state->globalConvoyId++;
    Print("\n========== GENERATING CONVOY #%d FROM TOWN 4 TO TOWN 5 ==========\n", state->globalConvoyId);
    
    // Prepare cargo from Town 4
    TownSupplies cargo;
    
    // Determine target cargo amount (10-20 tons)
    double targetCargo = Uniform(Config::MIN_CARGO_T4, Config::MAX_CARGO_T4);
    double availableSupplies = state->townStorage[3].getTotal();
    
    if (availableSupplies < targetCargo) {
        targetCargo = availableSupplies;
    }
    
    if (targetCargo <= 0) {
        Print("  No supplies available in Town 4, convoy cancelled\n");
        // Schedule next convoy anyway
        double nextInterval = Uniform(Config::CONVOY_INTERVAL_T4_MIN, Config::CONVOY_INTERVAL_T4_MAX);
        Activate(Time + nextInterval);
        return;
    }
    
    // Calculate wagons and mules needed
    int availableWagons = state->townStorage[3].wagons;
    int availableMules = state->townStorage[3].mules;
    
    int neededWagons = (int)(targetCargo / Config::WAGON_CAPACITY);
    if (neededWagons > availableWagons) neededWagons = availableWagons;
    
    double remainingCargo = targetCargo - (neededWagons * Config::WAGON_CAPACITY);
    int neededMules = (int)(remainingCargo / Config::MULE_CARRY_CAPACITY) + 1;
    if (neededMules > availableMules) neededMules = availableMules;
    
    double actualCapacity = neededWagons * Config::WAGON_CAPACITY + neededMules * Config::MULE_CARRY_CAPACITY;
    if (actualCapacity > targetCargo) actualCapacity = targetCargo;
    
    cargo.wagons = neededWagons;
    cargo.mules = neededMules;
    
    Print("  Preparing convoy: Target %.1f tons, Using %d wagons + %d mules (%.1f tons capacity)\n",
          targetCargo, neededWagons, neededMules, actualCapacity);
    
    // Load supplies proportionally
    double ratio = actualCapacity / availableSupplies;
    if (ratio > 1.0) ratio = 1.0;
    
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        cargo.supplies[i] = state->townStorage[3].supplies[i] * ratio;
    }
    
    Print("  Cargo prepared: %.1f tons (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n",
          cargo.getTotal(), cargo.supplies[GRAIN], cargo.supplies[FODDER],
          cargo.supplies[SALTED_MEAT], cargo.supplies[WINE_OIL], cargo.supplies[EQUIPMENT]);
    
    // Start convoy from Town 4 to Town 5
    (new Convoy(state->globalConvoyId, 3, 4, cargo))->Activate();
    
    // Schedule next convoy (every 5-6 hours)
    double nextInterval = Uniform(Config::CONVOY_INTERVAL_T4_MIN, Config::CONVOY_INTERVAL_T4_MAX);
    Activate(Time + nextInterval);
}
