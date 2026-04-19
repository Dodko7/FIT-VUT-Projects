#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    // ============================================================================
    // RUNTIME CONFIGURATION (modifiable via command-line arguments)
    // ============================================================================
    struct RuntimeConfig {
        bool weatherEnabled = true;
        bool robbersEnabled = true;
        bool experimentMode = false;
        int simulationDays = 100;
        int randomSeed = 12345;
    };
    
    // Global runtime configuration instance
    inline RuntimeConfig runtime;
    // ============================================================================
    // SIMULATION PARAMETERS
    // ============================================================================
    constexpr int NUM_TOWNS = 5;
    
    // ============================================================================
    // TRAVEL CONFIGURATION (all times in hours)
    // ============================================================================
    inline double travelTimes[] = {3.0*24.0, 1*24.0, 0.8*24.0, 3.0*24.0};  // Town 1->2, 2->3, 3->4, 4->5 (in hours)
    inline double travelCosts[] = {2500.0, 1500.0, 600.0, 300.0};  // Town 1->2, 2->3, 3->4, 4->5
    
    // ============================================================================
    // WAGON AND MULE CONFIGURATION
    // ============================================================================
    constexpr double WAGON_CAPACITY = 0.5;           // tons per wagon
    constexpr double MULE_CARRY_CAPACITY = 0.12;    // tons per mule
    
    // ============================================================================
    // TOWN 1 TO TOWN 2 CONVOY CONFIGURATION
    // ============================================================================
    constexpr double CONVOY_INTERVAL_T1 = (7.0 * 24.0) / 3.0;   // Hours between convoys (~56 hours, 3 per week)
    constexpr int WAGONS_PER_CONVOY_T1 = 100;    // Fixed composition
    constexpr int MULES_PER_CONVOY_T1 = 200;     // Fixed composition
    
    // ============================================================================
    // TOWN 4 TO TOWN 5 CONVOY CONFIGURATION
    // ============================================================================
    constexpr double MIN_CARGO_T4 = 10.0;            // Minimum cargo (tons)
    constexpr double MAX_CARGO_T4 = 20.0;            // Maximum cargo (tons)
    constexpr double CONVOY_INTERVAL_T4_MIN = 5.0;   // 5 hours
    constexpr double CONVOY_INTERVAL_T4_MAX = 6.0;   // 6 hours
    
    // ============================================================================
    // TOWN 5 CONSUMPTION CONFIGURATION
    // ============================================================================
    constexpr double CONSUMPTION_INTERVAL = 24.0;    // Consume every 24 hours (1 day)
    
    // ============================================================================
    // WEATHER SYSTEM CONFIGURATION (all times in hours)
    // ============================================================================
    constexpr double WEATHER_DELAY_MIN = 50.0;       // Minimum delay percentage
    constexpr double WEATHER_DELAY_MAX = 100.0;      // Maximum delay percentage
    constexpr double WEATHER_INITIAL_DELAY_MIN = 10.0 * 24.0;  // Min delay before first weather event (240 hours = 10 days)
    constexpr double WEATHER_INITIAL_DELAY_MAX = 20.0 * 24.0;  // Max delay before first weather event (480 hours = 20 days)
    
    // Active modifiable values (changed during experiments)
    inline double WEATHER_DURATION_MIN_ACTIVE = 2.0 * 24.0;     // Active minimum weather duration (48 hours = 2 days)
    inline double WEATHER_DURATION_MAX_ACTIVE = 5.0 * 24.0;     // Active maximum weather duration (120 hours = 5 days)
    inline double WEATHER_INTERVAL_MIN_ACTIVE = 10.0 * 24.0;     // Active minimum interval between weather events (168 hours = 7 days)
    inline double WEATHER_INTERVAL_MAX_ACTIVE = 15.0 * 24.0;    // Active maximum interval between weather events (336 hours = 14 days)
    inline double CONSUMPTION_PER_DAY_ACTIVE = 30.0;     // Active consumption per day (tons) - THIS IS WHAT THE SIMULATION USES
    
    // ============================================================================
    // GRAPH DISPLAY CONFIGURATION
    // ============================================================================
    constexpr double SUPPLY_GRAPH_MAX = 1000.0;      // Maximum supply for Town 5 graph Y-axis scaling
    constexpr int SUPPLY_GRAPH_HEIGHT = 30;          // Height of Town 5 supply graph
    constexpr int SUPPLY_GRAPH_WIDTH = 100;          // Width of Town 5 supply graph
    constexpr int COST_GRAPH_HEIGHT = 30;            // Height of campaign cost graph
    constexpr int COST_GRAPH_WIDTH = 100;            // Width of campaign cost graph
    constexpr int DAILY_COST_GRAPH_HEIGHT = 20;      // Height of daily cost graph
    constexpr int DAILY_COST_GRAPH_WIDTH = 100;      // Width of daily cost graph
    constexpr int TOWN1_WEEKLY_COST_GRAPH_HEIGHT = 20;  // Height of Town 1 weekly cost graph
    constexpr int TOWN1_WEEKLY_COST_GRAPH_WIDTH = 15;   // Width of Town 1 weekly cost graph (weeks)
    constexpr int ALL_TOWNS_WEEKLY_COST_GRAPH_HEIGHT = 25;  // Height of all towns weekly cost graph
    constexpr int ALL_TOWNS_WEEKLY_COST_GRAPH_WIDTH = 15;   // Width of all towns weekly cost graph (weeks)
    constexpr int DELIVERIES_GRAPH_HEIGHT = 20;          // Height of Town 5 deliveries graph
    constexpr int DELIVERIES_GRAPH_WIDTH = 100;          // Width of Town 5 deliveries graph
    
    // ============================================================================
    // EXPERIMENT MODE CONFIGURATION
    // ============================================================================
    constexpr bool EXP_WEATHER_DURATION = false;         // Vary weather duration exponentially
    constexpr bool EXP_WEATHER_INTERVAL = false;        // Vary weather interval exponentially
    constexpr bool EXP_CONSUMPTION = false;             // Vary consumption rate exponentially
    constexpr int EXPERIMENT_RUNS = 10;                 // Number of simulation runs in experiment mode
    constexpr double EXPERIMENT_MULTIPLIER = 10.0;       // Exponential growth factor between runs
    
    // ============================================================================
    // ROBBER SYSTEM CONFIGURATION
    // ============================================================================
    inline double robberProbability[] = {0.5, 0.8, 0.5, 0.2};  // Attack probability per route (Town 1->2, 2->3, 3->4, 4->5)
    inline double robberStealMin[] = {10.0, 8.0, 5.0, 3.0};        // Minimum % stolen per route
    inline double robberStealMax[] = {30.0, 20.0, 15.0, 10.0};     // Maximum % stolen per route
}

#endif // CONFIG_H
