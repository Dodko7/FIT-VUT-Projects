#ifndef TOWN_SUPPLIES_H
#define TOWN_SUPPLIES_H

#include "SupplyTypes.h"

class TownSupplies {
public:
    double supplies[NUM_SUPPLY_TYPES];
    int wagons;
    int mules;
    
    TownSupplies();
    
    double getTotal() const;
    void add(const TownSupplies& other);
};

#endif // TOWN_SUPPLIES_H
