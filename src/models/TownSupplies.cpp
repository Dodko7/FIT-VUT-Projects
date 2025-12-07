#include "TownSupplies.h"

TownSupplies::TownSupplies() : wagons(0), mules(0) {
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        supplies[i] = 0.0;
    }
}

double TownSupplies::getTotal() const {
    double total = 0.0;
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        total += supplies[i];
    }
    return total;
}

void TownSupplies::add(const TownSupplies& other) {
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        supplies[i] += other.supplies[i];
    }
    wagons += other.wagons;
    mules += other.mules;
}
