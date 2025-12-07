#ifndef CONVOY_GENERATORS_H
#define CONVOY_GENERATORS_H

#include <simlib.h>

class Town1ConvoyGenerator : public Event {
public:
    void Behavior() override;
};

class Town4ConvoyGenerator : public Event {
public:
    void Behavior() override;
};

#endif // CONVOY_GENERATORS_H
