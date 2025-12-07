#include "WeatherEvents.h"
#include "../services/SimulationState.h"
#include "../config/Config.h"

void BadWeatherEvent::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    state->currentWeatherDelayPercentage = Uniform(Config::WEATHER_DELAY_MIN, Config::WEATHER_DELAY_MAX);
    state->weatherDelayMultiplier = 1.0 + (state->currentWeatherDelayPercentage / 100.0);
    state->isWeatherActive = true;
    
    Print("[TIME %.2f] *** BAD WEATHER STARTS ***\n", Time);
    Print("              Delay: +%.1f%% | Multiplier: %.2fx\n\n", 
          state->currentWeatherDelayPercentage, state->weatherDelayMultiplier);
    
    // Schedule weather clearing
    (new WeatherClearEvent)->Activate(Time + Uniform(Config::WEATHER_DURATION_MIN_ACTIVE, 
                                                      Config::WEATHER_DURATION_MAX_ACTIVE));
}

void WeatherClearEvent::Behavior() {
    auto* state = SimulationState::GetInstance();
    
    state->weatherDelayMultiplier = 1.0;
    state->currentWeatherDelayPercentage = 0.0;
    state->isWeatherActive = false;
    
    Print("[TIME %.2f] >>> WEATHER CLEARED <<<\n", Time);
    Print("              Travel times back to normal\n\n");
    
    // Schedule next bad weather
    (new BadWeatherEvent)->Activate(Time + Uniform(Config::WEATHER_INTERVAL_MIN_ACTIVE, 
                                                     Config::WEATHER_INTERVAL_MAX_ACTIVE));
}
