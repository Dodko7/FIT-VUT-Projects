#ifndef CONVOY_H
#define CONVOY_H

#include <simlib.h>
#include "../models/TownSupplies.h"

class Convoy : public Process {
private:
    int convoyId;
    int fromTown;  // 0-based index
    int toTown;    // 0-based index
    TownSupplies cargo;
    
public:
    Convoy(int id, int from, int to, const TownSupplies& supplies);
    void Behavior() override;
};

#endif // CONVOY_H
