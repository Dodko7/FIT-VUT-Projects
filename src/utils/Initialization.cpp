#include "Initialization.h"
#include "../services/SimulationState.h"
#include "../config/Config.h"
#include "../models/SupplyTypes.h"
#include <simlib.h>

void Initialization::InitializeFacilities() {
    auto* state = SimulationState::GetInstance();
    
    for (int i = 0; i < Config::NUM_TOWNS; i++) {
        state->towns.push_back(new Facility(("Town" + std::to_string(i+1)).c_str()));
    }
}

void Initialization::InitializeTownSupplies() {
    auto* state = SimulationState::GetInstance();
    
    // Town 1: Unlimited supplies (very large amounts)
    state->townStorage[0].supplies[GRAIN] = 100000.0;
    state->townStorage[0].supplies[FODDER] = 100000.0;
    state->townStorage[0].supplies[SALTED_MEAT] = 100000.0;
    state->townStorage[0].supplies[WINE_OIL] = 100000.0;
    state->townStorage[0].supplies[EQUIPMENT] = 100000.0;
    state->townStorage[0].wagons = 100000;
    state->townStorage[0].mules = 100000;
    
    // Towns 2-4: Small initial supplies
    for (int i = 1; i < 4; i++) {
        state->townStorage[i].supplies[GRAIN] = Uniform(20, 50);
        state->townStorage[i].supplies[FODDER] = Uniform(10, 30);
        state->townStorage[i].supplies[SALTED_MEAT] = Uniform(5, 15);
        state->townStorage[i].supplies[WINE_OIL] = Uniform(5, 15);
        state->townStorage[i].supplies[EQUIPMENT] = Uniform(2, 10);
        state->townStorage[i].wagons = (int)Uniform(10, 30);
        state->townStorage[i].mules = (int)Uniform(20, 50);
    }
    
    // Town 5: Starting supplies to avoid initial starvation
    state->townStorage[4].supplies[GRAIN] = 500.0;
    state->townStorage[4].supplies[FODDER] = 200.0;
    state->townStorage[4].supplies[SALTED_MEAT] = 60.0;
    state->townStorage[4].supplies[WINE_OIL] = 60.0;
    state->townStorage[4].supplies[EQUIPMENT] = 30.0;
    state->townStorage[4].wagons = 0;
    state->townStorage[4].mules = 0;
}
