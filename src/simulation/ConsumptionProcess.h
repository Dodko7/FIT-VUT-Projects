#ifndef CONSUMPTION_PROCESS_H
#define CONSUMPTION_PROCESS_H

#include <simlib.h>

class DailyConsumptionProcess : public Process {
public:
    void Behavior() override;
};

#endif // CONSUMPTION_PROCESS_H
