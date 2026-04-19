#ifndef WEATHER_EVENTS_H
#define WEATHER_EVENTS_H

#include <simlib.h>

class BadWeatherEvent : public Event {
public:
    void Behavior() override;
};

class WeatherClearEvent : public Event {
public:
    void Behavior() override;
};

#endif // WEATHER_EVENTS_H
