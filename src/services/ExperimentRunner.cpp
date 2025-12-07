#include "ExperimentRunner.h"
#include "SimulationState.h"
#include "GraphGenerator.h"
#include "../config/Config.h"
#include "../models/ExperimentResult.h"
#include "../simulation/WeatherEvents.h"
#include "../simulation/ConvoyGenerators.h"
#include "../simulation/ConsumptionProcess.h"
#include "../utils/Initialization.h"
#include <simlib.h>
#include <cmath>

void ExperimentRunner::RunExperiments() {
    auto* state = SimulationState::GetInstance();
    
    std::string experimentType;
    
    if (Config::EXP_WEATHER_DURATION) {
        experimentType = "Weather Duration (exponential)";
        Print("Running %d simulations with exponentially increasing weather duration\n", Config::EXPERIMENT_RUNS);
        Print("Base duration: %.2f-%.2f hours (%.1f-%.1f days)\n", 2.0*24.0, 5.0*24.0, 2.0, 5.0);
        Print("Multiplier: %.2f\n\n", Config::EXPERIMENT_MULTIPLIER);
        
        double baseWeatherDurationMin = 2.0 * 24.0;  // 48 hours
        double baseWeatherDurationMax = 5.0 * 24.0;  // 120 hours
        
        for (int run = 0; run < Config::EXPERIMENT_RUNS; run++) {
            double multiplier = std::pow(Config::EXPERIMENT_MULTIPLIER, run);
            Config::WEATHER_DURATION_MIN_ACTIVE = baseWeatherDurationMin * multiplier;
            Config::WEATHER_DURATION_MAX_ACTIVE = baseWeatherDurationMax * multiplier;
            
            Print("\n=== RUN %d/%d ===\n", run + 1, Config::EXPERIMENT_RUNS);
            Print("Weather duration: %.2f-%.2f hours (%.1f-%.1f days)\n", 
                  Config::WEATHER_DURATION_MIN_ACTIVE, Config::WEATHER_DURATION_MAX_ACTIVE,
                  Config::WEATHER_DURATION_MIN_ACTIVE/24.0, Config::WEATHER_DURATION_MAX_ACTIVE/24.0);
            
            state->Reset();
            
            Initialization::InitializeTownSupplies();
            Init(0, Config::runtime.simulationDays * 24.0);  // Convert days to hours
            
            (new Town1ConvoyGenerator)->Activate();
            (new Town4ConvoyGenerator)->Activate();
            (new DailyConsumptionProcess)->Activate();
            
            if (Config::runtime.weatherEnabled) {
                (new BadWeatherEvent)->Activate(Time + Uniform(Config::WEATHER_INITIAL_DELAY_MIN, Config::WEATHER_INITIAL_DELAY_MAX));
            }
            
            Run();
            
            ExperimentResult result;
            result.parameterValue = Config::WEATHER_DURATION_MIN_ACTIVE;
            result.totalCost = state->campaignCost;
            result.finalSupplies = state->townStorage[4].getTotal();
            result.convoyCount = (int)state->ConvoyCountTown1.Number();
            state->experimentResults.push_back(result);
            
            Print("Run %d complete - Cost: %.2f, Final supplies: %.2f\n", 
                  run + 1, state->campaignCost, state->townStorage[4].getTotal());
        }
        
    } else if (Config::EXP_WEATHER_INTERVAL) {
        experimentType = "Weather Interval (exponential)";
        Print("Running %d simulations with exponentially increasing weather interval\n", Config::EXPERIMENT_RUNS);
        Print("Base interval: %.2f-%.2f hours (%.1f-%.1f days)\n", 7.0*24.0, 14.0*24.0, 7.0, 14.0);
        Print("Multiplier: %.2f\n\n", Config::EXPERIMENT_MULTIPLIER);
        
        double baseWeatherIntervalMin = 7.0 * 24.0;   // 168 hours
        double baseWeatherIntervalMax = 14.0 * 24.0;  // 336 hours
        
        for (int run = 0; run < Config::EXPERIMENT_RUNS; run++) {
            double multiplier = std::pow(Config::EXPERIMENT_MULTIPLIER, run);
            Config::WEATHER_INTERVAL_MIN_ACTIVE = baseWeatherIntervalMin * multiplier;
            Config::WEATHER_INTERVAL_MAX_ACTIVE = baseWeatherIntervalMax * multiplier;
            
            Print("\n=== RUN %d/%d ===\n", run + 1, Config::EXPERIMENT_RUNS);
            Print("Weather interval: %.2f-%.2f hours (%.1f-%.1f days)\n", 
                  Config::WEATHER_INTERVAL_MIN_ACTIVE, Config::WEATHER_INTERVAL_MAX_ACTIVE,
                  Config::WEATHER_INTERVAL_MIN_ACTIVE/24.0, Config::WEATHER_INTERVAL_MAX_ACTIVE/24.0);
            
            state->Reset();
            
            Initialization::InitializeTownSupplies();
            Init(0, Config::runtime.simulationDays * 24.0);  // Convert days to hours
            
            (new Town1ConvoyGenerator)->Activate();
            (new Town4ConvoyGenerator)->Activate();
            (new DailyConsumptionProcess)->Activate();
            
            if (Config::runtime.weatherEnabled) {
                (new BadWeatherEvent)->Activate(Time + Uniform(Config::WEATHER_INITIAL_DELAY_MIN, Config::WEATHER_INITIAL_DELAY_MAX));
            }
            
            Run();
            
            ExperimentResult result;
            result.parameterValue = Config::WEATHER_INTERVAL_MIN_ACTIVE;
            result.totalCost = state->campaignCost;
            result.finalSupplies = state->townStorage[4].getTotal();
            result.convoyCount = (int)state->ConvoyCountTown1.Number();
            state->experimentResults.push_back(result);
            
            Print("Run %d complete - Cost: %.2f, Final supplies: %.2f\n", 
                  run + 1, state->campaignCost, state->townStorage[4].getTotal());
        }
        
    } else if (Config::EXP_CONSUMPTION) {
        experimentType = "Consumption Rate (exponential)";
        Print("Running %d simulations with exponentially increasing consumption rate\n", Config::EXPERIMENT_RUNS);
        Print("Base consumption: %.2f tons/day\n", 30.0);
        Print("Multiplier: %.2f\n\n", Config::EXPERIMENT_MULTIPLIER);
        
        for (int run = 0; run < Config::EXPERIMENT_RUNS; run++) {
            double multiplier = std::pow(Config::EXPERIMENT_MULTIPLIER, run);
            Config::CONSUMPTION_PER_DAY_ACTIVE = 30.0 * multiplier;
            
            Print("\n=== RUN %d/%d ===\n", run + 1, Config::EXPERIMENT_RUNS);
            Print("Consumption rate: %.2f tons/day\n", Config::CONSUMPTION_PER_DAY_ACTIVE);
            
            state->Reset();
            
            Initialization::InitializeTownSupplies();
            Init(0, Config::runtime.simulationDays * 24.0);  // Convert days to hours
            
            (new Town1ConvoyGenerator)->Activate();
            (new Town4ConvoyGenerator)->Activate();
            (new DailyConsumptionProcess)->Activate();
            
            if (Config::runtime.weatherEnabled) {
                (new BadWeatherEvent)->Activate(Time + Uniform(Config::WEATHER_INITIAL_DELAY_MIN, Config::WEATHER_INITIAL_DELAY_MAX));
            }
            
            Run();
            
            ExperimentResult result;
            result.parameterValue = Config::CONSUMPTION_PER_DAY_ACTIVE;
            result.totalCost = state->campaignCost;
            result.finalSupplies = state->townStorage[4].getTotal();
            result.convoyCount = (int)state->ConvoyCountTown1.Number();
            state->experimentResults.push_back(result);
            
            Print("Run %d complete - Cost: %.2f, Final supplies: %.2f\n", 
                  run + 1, state->campaignCost, state->townStorage[4].getTotal());
        }
    }
    
    GraphGenerator::GenerateExperimentGraph(experimentType);
    
    Print("\n========================================\n");
    Print("  EXPERIMENT COMPLETED\n");
    Print("========================================\n");
    Print("Results saved to: experiment_results.txt\n");
}
