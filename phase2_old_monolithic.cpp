#include <simlib.h>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>

// ============================================================================
// SUPPLY TYPES
// ============================================================================

enum SupplyType {
    GRAIN,           // Wheat/Barley combined
    FODDER,          // Animal fodder
    SALTED_MEAT,     // Salted meat
    WINE_OIL,        // Wine/Oil  
    EQUIPMENT,       // Equipment/Clothing
    NUM_SUPPLY_TYPES
};

const char* SupplyNames[] = {
    "Grain",
    "Fodder",
    "Salted Meat",
    "Wine/Oil",
    "Equipment"
};

// ============================================================================
// CONFIGURATION
// ============================================================================

const int NUM_TOWNS = 5;
const int SIMULATION_DAYS = 100;             // Total days to simulate

// Travel times between consecutive towns (in days)
double travelTimes[] = {5.0, 1.5, 2.0, 6.0};  // Town 1->2, 2->3, 3->4, 4->5

// Travel costs per convoy between consecutive towns
double travelCosts[] = {2500.0, 1500.0, 600.0, 300.0};  // Town 1->2, 2->3, 3->4, 4->5

// Wagon and mule configuration
const double WAGON_CAPACITY = 0.5;           // tons per wagon
const double MULE_CARRY_CAPACITY = 0.12;     // tons per mule

// Town 1 to Town 2 convoy configuration
const int CONVOYS_PER_WEEK_T1 = 3;           // Modifiable: convoys sent from Town 1 per week
const double DAYS_PER_WEEK = 7.0;
const double CONVOY_INTERVAL_T1 = DAYS_PER_WEEK / CONVOYS_PER_WEEK_T1;  // Days between convoys

// Town 1 to Town 2 convoy composition (fixed)
const int WAGONS_PER_CONVOY_T1 = 100;
const int MULES_PER_CONVOY_T1 = 200;

// Town 4 to Town 5 convoy configuration
const double MIN_CARGO_T4 = 10.0;            // tons
const double MAX_CARGO_T4 = 20.0;            // tons
const double CONVOY_INTERVAL_T4_MIN = 5.0 / 24.0;   // 5 hours in days
const double CONVOY_INTERVAL_T4_MAX = 6.0 / 24.0;   // 6 hours in days

// Town 5 consumption configuration
const double CONSUMPTION_PER_DAY = 30.0;     // tons per day
const double CONSUMPTION_PER_WEEK = 210.0;   // tons per week

// Weather system configuration (modifiable for experiments)
const bool WEATHER_ENABLED = true;           // Enable/disable bad weather events
const double WEATHER_DELAY_MIN = 50.0;       // Minimum delay percentage
const double WEATHER_DELAY_MAX = 100.0;      // Maximum delay percentage
const double WEATHER_INITIAL_DELAY_MIN = 10.0;  // Min delay before first weather event (days)
const double WEATHER_INITIAL_DELAY_MAX = 20.0;  // Max delay before first weather event (days)

// Active modifiable values (changed during experiments)
double WEATHER_DURATION_MIN_ACTIVE = 2.0;     // Active minimum weather duration (days)
double WEATHER_DURATION_MAX_ACTIVE = 5.0;     // Active maximum weather duration (days)
double WEATHER_INTERVAL_MIN_ACTIVE = 7.0;     // Active minimum interval between weather events (days)
double WEATHER_INTERVAL_MAX_ACTIVE = 14.0;    // Active maximum interval between weather events (days)
double CONSUMPTION_PER_DAY_ACTIVE = 30.0;     // Active consumption per day (tons)

// Graph display configuration
const double SUPPLY_GRAPH_MAX = 1000.0;      // Maximum supply for Town 5 graph Y-axis scaling
const int SUPPLY_GRAPH_HEIGHT = 30;          // Height of Town 5 supply graph
const int SUPPLY_GRAPH_WIDTH = 100;          // Width of Town 5 supply graph
const int COST_GRAPH_HEIGHT = 30;            // Height of campaign cost graph
const int COST_GRAPH_WIDTH = 100;            // Width of campaign cost graph
const int WEEKLY_GRAPH_HEIGHT = 20;          // Height of weekly cost graphs
const int WEEKLY_GRAPH_WIDTH = 7;            // Width of weekly cost graphs (days)
const int SUMMARY_GRAPH_HEIGHT = 10;         // Height of weekly summary graph
const double SUMMARY_GRAPH_MIN_COST = 15000.0;  // Min Y-axis for weekly summary
const double SUMMARY_GRAPH_MAX_COST = 25000.0;  // Max Y-axis for weekly summary

// Experiment mode configuration
const bool EXPERIMENT_MODE = false;              // Enable/disable experiment mode (multiple runs)
const bool EXP_WEATHER_DURATION = false;         // Vary weather duration exponentially
const bool EXP_WEATHER_INTERVAL = false;        // Vary weather interval exponentially
const bool EXP_CONSUMPTION = false;             // Vary consumption rate exponentially
const int EXPERIMENT_RUNS = 10;                 // Number of simulation runs in experiment mode
const double EXPERIMENT_MULTIPLIER = 10.0;       // Exponential growth factor between runs

// Robber system configuration
const bool ROBBERS_ENABLED = true;                // Enable/disable robber attacks
double robberProbability[] = {0.5, 0.8, 0.5, 0.2};  // Attack probability per route (Town 1->2, 2->3, 3->4, 4->5)
double robberStealMin[] = {10.0, 8.0, 5.0, 3.0};        // Minimum % stolen per route
double robberStealMax[] = {30.0, 20.0, 15.0, 10.0};     // Maximum % stolen per route

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct TownSupplies {
    double supplies[NUM_SUPPLY_TYPES];
    int wagons;
    int mules;
    
    TownSupplies() : wagons(0), mules(0) {
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            supplies[i] = 0.0;
        }
    }
    
    double getTotal() const {
        double total = 0.0;
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            total += supplies[i];
        }
        return total;
    }
    
    void add(const TownSupplies& other) {
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            supplies[i] += other.supplies[i];
        }
        wagons += other.wagons;
        mules += other.mules;
    }
};

// ============================================================================
// GLOBAL STATE
// ============================================================================

std::vector<Facility*> towns;
TownSupplies townStorage[NUM_TOWNS];

Stat ConvoyCountTown1("Convoys from Town 1");
Stat ConvoyCountTown4("Convoys from Town 4");
Stat TotalDeliveredToTown5("Total delivered to Town 5 (tons)");

int globalConvoyId = 0;

// Graph data
std::vector<double> town5SuppliesHistory;
std::ofstream graphFile;

// Economics tracking
double campaignCost = 0.0;
std::vector<double> campaignCostHistory;
std::ofstream costGraphFile;

// Weekly cost tracking
std::vector<double> weeklyCosts;  // Cost at end of each week
std::ofstream weeklyCostGraphFile;
double lastWeekCost = 0.0;

// Weather system
double weatherDelayMultiplier = 1.0;
double currentWeatherDelayPercentage = 0.0;
bool isWeatherActive = false;

// Robber system tracking
int totalRobberAttacks = 0;
double totalSuppliesStolen = 0.0;
Stat RobberAttacksPerRoute[4] = {
    Stat("Robber attacks Town 1->2"),
    Stat("Robber attacks Town 2->3"),
    Stat("Robber attacks Town 3->4"),
    Stat("Robber attacks Town 4->5")
};

// Experiment mode data
struct ExperimentResult {
    double parameterValue;      // The varied parameter value
    double totalCost;           // Total campaign cost
    double finalSupplies;       // Final Town 5 supplies
    int convoyCount;            // Total convoys
};
std::vector<ExperimentResult> experimentResults;

// ============================================================================
// INITIALIZATION
// ============================================================================

void InitializeFacilities() {
    for (int i = 0; i < NUM_TOWNS; i++) {
        std::string name = "Town " + std::to_string(i + 1);
        towns.push_back(new Facility(name.c_str()));
    }
}

void InitializeTownSupplies() {
    // Town 1: Unlimited supplies (very large amounts)
    townStorage[0].supplies[GRAIN] = 100000.0;
    townStorage[0].supplies[FODDER] = 100000.0;
    townStorage[0].supplies[SALTED_MEAT] = 100000.0;
    townStorage[0].supplies[WINE_OIL] = 100000.0;
    townStorage[0].supplies[EQUIPMENT] = 100000.0;
    townStorage[0].wagons = 100000;  // Unlimited wagons
    townStorage[0].mules = 100000;   // Unlimited mules
    
    // Towns 2-4: Small initial supplies
    for (int i = 1; i < 4; i++) {
        townStorage[i].supplies[GRAIN] = Uniform(20, 50);
        townStorage[i].supplies[FODDER] = Uniform(10, 30);
        townStorage[i].supplies[SALTED_MEAT] = Uniform(5, 15);
        townStorage[i].supplies[WINE_OIL] = Uniform(5, 15);
        townStorage[i].supplies[EQUIPMENT] = Uniform(2, 10);
        townStorage[i].wagons = (int)Uniform(10, 30);
        townStorage[i].mules = (int)Uniform(20, 50);
    }
    
    // Town 5: Starting supplies to avoid initial starvation
    // Enough for about 7-10 days of consumption (~635 tons per week)
    townStorage[4].supplies[GRAIN] = 500.0;
    townStorage[4].supplies[FODDER] = 200.0;
    townStorage[4].supplies[SALTED_MEAT] = 60.0;
    townStorage[4].supplies[WINE_OIL] = 60.0;
    townStorage[4].supplies[EQUIPMENT] = 30.0;
    townStorage[4].wagons = 0;
    townStorage[4].mules = 0;
}

// ============================================================================
// WEATHER EVENTS
// ============================================================================

class BadWeatherEvent : public Event {
    void Behavior();
};

class WeatherClearEvent : public Event {
    void Behavior();
};

void BadWeatherEvent::Behavior() {
    currentWeatherDelayPercentage = Uniform(WEATHER_DELAY_MIN, WEATHER_DELAY_MAX);
    weatherDelayMultiplier = 1.0 + (currentWeatherDelayPercentage / 100.0);
    isWeatherActive = true;
    
    Print("[TIME %.2f] *** BAD WEATHER STARTS ***\n", Time);
    Print("              Delay: +%.1f%% | Multiplier: %.2fx\n\n", 
          currentWeatherDelayPercentage, weatherDelayMultiplier);
    
    (new WeatherClearEvent)->Activate(Time + Uniform(WEATHER_DURATION_MIN_ACTIVE, WEATHER_DURATION_MAX_ACTIVE));
}

void WeatherClearEvent::Behavior() {
    weatherDelayMultiplier = 1.0;
    currentWeatherDelayPercentage = 0.0;
    isWeatherActive = false;
    
    Print("[TIME %.2f] >>> WEATHER CLEARED <<<\n", Time);
    Print("              Travel times back to normal\n\n");
    
    (new BadWeatherEvent)->Activate(Time + Uniform(WEATHER_INTERVAL_MIN_ACTIVE, WEATHER_INTERVAL_MAX_ACTIVE));
}

// ============================================================================
// UNIFORM CONVOY PROCESS CLASS
// ============================================================================

class Convoy : public Process {
    int convoyId;
    int fromTown;  // 0-based index
    int toTown;    // 0-based index
    TownSupplies cargo;
    
    void Behavior();
public:
    Convoy(int id, int from, int to, const TownSupplies& supplies) 
        : convoyId(id), fromTown(from), toTown(to), cargo(supplies) {}
};

// ============================================================================
// UNIFORM CONVOY BEHAVIOR IMPLEMENTATION
// ============================================================================

void Convoy::Behavior() {
    Print("\n[TIME %.2f] Convoy #%d: Journey from Town %d to Town %d\n", 
          Time, convoyId, fromTown + 1, toTown + 1);
    
    // LOADING PHASE - Seize town, load cargo, release town
    Seize(*towns[fromTown]);
    
    Print("  Seizing Town %d for loading...\n", fromTown + 1);
    
    for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
        townStorage[fromTown].supplies[i] -= cargo.supplies[i];
    }
    townStorage[fromTown].wagons -= cargo.wagons;
    townStorage[fromTown].mules -= cargo.mules;
    
    Print("  Loading: %.1f tons with %d wagons and %d mules\n", 
          cargo.getTotal(), cargo.wagons, cargo.mules);
    Print("    Cargo breakdown: G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f\n",
          cargo.supplies[GRAIN], cargo.supplies[FODDER],
          cargo.supplies[SALTED_MEAT], cargo.supplies[WINE_OIL], cargo.supplies[EQUIPMENT]);
    Print("  Town %d after loading: %.1f tons remaining\n", 
          fromTown + 1, townStorage[fromTown].getTotal());
    
    Release(*towns[fromTown]);
    Print("  Released Town %d\n", fromTown + 1);
    
    // TRAVEL PHASE
    double baseTravelTime = Uniform(travelTimes[fromTown] * 0.9, travelTimes[fromTown] * 1.1);
    double travelTime = baseTravelTime * weatherDelayMultiplier;
    double travelCost = travelCosts[fromTown];
    campaignCost += travelCost;
    
    const char* weatherStatus = isWeatherActive ? " [BAD WEATHER]" : "";
    Print("  Traveling to Town %d...%s (Base: %.2f days, Weather: %.2fx, Actual: %.2f days)\n",
          toTown + 1, weatherStatus, baseTravelTime, weatherDelayMultiplier, travelTime);
    Print("  Cost: %.2f, Total campaign cost: %.2f\n", travelCost, campaignCost);
    Wait(travelTime);
    
    // ROBBER ATTACK CHECK (during travel)
    if (ROBBERS_ENABLED) {
        double attackChance = Random();
        if (attackChance < robberProbability[fromTown]) {
            double stealPercentage = Uniform(robberStealMin[fromTown], robberStealMax[fromTown]);
            double cargoBeforeAttack = cargo.getTotal();
            
            // Steal percentage from each supply type
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                double stolen = cargo.supplies[i] * (stealPercentage / 100.0);
                cargo.supplies[i] -= stolen;
            }
            
            double cargoAfterAttack = cargo.getTotal();
            double actualStolen = cargoBeforeAttack - cargoAfterAttack;
            
            totalRobberAttacks++;
            totalSuppliesStolen += actualStolen;
            RobberAttacksPerRoute[fromTown](1);
            
            Print("\n  *** ROBBER ATTACK! ***\n");
            Print("  Route: Town %d -> Town %d\n", fromTown + 1, toTown + 1);
            Print("  Stolen: %.1f%% (%.2f tons)\n", stealPercentage, actualStolen);
            Print("  Cargo remaining: %.2f tons\n\n", cargoAfterAttack);
        }
    }
    
    // ARRIVAL AND UNLOADING PHASE - Seize town, unload cargo, release town
    Print("[TIME %.2f] Convoy #%d: Arrived at Town %d\n", Time, convoyId, toTown + 1);
    
    Seize(*towns[toTown]);
    Print("  Seizing Town %d for unloading...\n", toTown + 1);
    
    Print("  Unloading %.1f tons at Town %d\n", cargo.getTotal(), toTown + 1);
    
    townStorage[toTown].add(cargo);
    
    Print("  Town %d now has: %.1f tons total (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n", 
          toTown + 1, townStorage[toTown].getTotal(),
          townStorage[toTown].supplies[GRAIN], townStorage[toTown].supplies[FODDER],
          townStorage[toTown].supplies[SALTED_MEAT], townStorage[toTown].supplies[WINE_OIL],
          townStorage[toTown].supplies[EQUIPMENT]);
    
    Release(*towns[toTown]);
    Print("  Released Town %d\n", toTown + 1);
    
    // Record statistics
    if (fromTown == 0) {
        ConvoyCountTown1(1);
    } else if (fromTown == 3) {
        TotalDeliveredToTown5(cargo.getTotal());
        ConvoyCountTown4(1);
    }
    
    // CHAIN REACTION: Generate next convoy if not at Town 4 or Town 5
    if (toTown >= 1 && toTown <= 3) {
        globalConvoyId++;
        Print("\n  >>> Generating next convoy #%d from Town %d to Town %d <<<\n", 
              globalConvoyId, toTown + 1, toTown + 2);
        
        // Prepare cargo from current destination town
        TownSupplies nextCargo;
        
        double availableSupplies = townStorage[toTown].getTotal();
        
        if (availableSupplies > 0) {
            // Calculate wagons and mules needed
            int availableWagons = townStorage[toTown].wagons;
            int availableMules = townStorage[toTown].mules;
            
            // Use all available supplies for intermediate towns (2 and 3)
            // For town 4, we'll let the Town4ConvoyGenerator handle it
            double targetCargo = availableSupplies;
            
            int neededWagons = (int)(targetCargo / WAGON_CAPACITY);
            if (neededWagons > availableWagons) neededWagons = availableWagons;
            
            double remainingCargo = targetCargo - (neededWagons * WAGON_CAPACITY);
            int neededMules = (int)(remainingCargo / MULE_CARRY_CAPACITY) + 1;
            if (neededMules > availableMules) neededMules = availableMules;
            
            double actualCapacity = neededWagons * WAGON_CAPACITY + neededMules * MULE_CARRY_CAPACITY;
            if (actualCapacity > targetCargo) actualCapacity = targetCargo;
            
            nextCargo.wagons = neededWagons;
            nextCargo.mules = neededMules;
            
            // Load supplies proportionally
            double ratio = actualCapacity / availableSupplies;
            if (ratio > 1.0) ratio = 1.0;
            
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                nextCargo.supplies[i] = townStorage[toTown].supplies[i] * ratio;
            }
            
            Print("  Next convoy will carry %.1f tons with %d wagons and %d mules\n",
                  nextCargo.getTotal(), nextCargo.wagons, nextCargo.mules);
            
            // Activate next convoy
            (new Convoy(globalConvoyId, toTown, toTown + 1, nextCargo))->Activate();
        } else {
            Print("  No supplies available in Town %d - chain broken\n", toTown + 1);
        }
    }
    
    Print("  Convoy #%d process ending.\n", convoyId);
}

// ============================================================================
// CONVOY GENERATORS
// ============================================================================

class Town1ConvoyGenerator : public Event {
    void Behavior() {
        globalConvoyId++;
        Print("\n========== GENERATING CONVOY #%d FROM TOWN 1 TO TOWN 2 ==========\n", globalConvoyId);
        
        // Prepare cargo from Town 1
        TownSupplies cargo;
        
        // Fixed composition: 100 wagons + 200 mules
        int wagons = WAGONS_PER_CONVOY_T1;
        int mules = MULES_PER_CONVOY_T1;
        
        double wagonCapacity = wagons * WAGON_CAPACITY;
        double muleCapacity = mules * MULE_CARRY_CAPACITY;
        double totalCapacity = wagonCapacity + muleCapacity;
        
        Print("  Preparing convoy: %d wagons (%.1f tons) + %d mules (%.1f tons) = %.1f tons capacity\n",
              wagons, wagonCapacity, mules, muleCapacity, totalCapacity);
        
        // Load supplies with specified ranges
        cargo.supplies[GRAIN] = Uniform(40, 50);
        cargo.supplies[FODDER] = Uniform(10, 15);
        cargo.supplies[SALTED_MEAT] = Uniform(3, 6);
        cargo.supplies[WINE_OIL] = Uniform(3, 6);
        cargo.supplies[EQUIPMENT] = Uniform(0.5, 2);
        cargo.wagons = wagons;
        cargo.mules = mules;
        
        Print("  Cargo prepared: %.1f tons (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n",
              cargo.getTotal(), cargo.supplies[GRAIN], cargo.supplies[FODDER], 
              cargo.supplies[SALTED_MEAT], cargo.supplies[WINE_OIL], cargo.supplies[EQUIPMENT]);
        
        // Start convoy from Town 1 to Town 2
        (new Convoy(globalConvoyId, 0, 1, cargo))->Activate();
        
        // Schedule next convoy
        Activate(Time + CONVOY_INTERVAL_T1);
    }
};

class Town4ConvoyGenerator : public Event {
    void Behavior() {
        globalConvoyId++;
        Print("\n========== GENERATING CONVOY #%d FROM TOWN 4 TO TOWN 5 ==========\n", globalConvoyId);
        
        // Prepare cargo from Town 4
        TownSupplies cargo;
        
        // Determine target cargo amount (10-20 tons)
        double targetCargo = Uniform(MIN_CARGO_T4, MAX_CARGO_T4);
        double availableSupplies = townStorage[3].getTotal();
        
        if (availableSupplies < targetCargo) {
            targetCargo = availableSupplies;
        }
        
        if (targetCargo <= 0) {
            Print("  No supplies available in Town 4, convoy cancelled\n");
            // Schedule next convoy anyway
            double nextInterval = Uniform(CONVOY_INTERVAL_T4_MIN, CONVOY_INTERVAL_T4_MAX);
            Activate(Time + nextInterval);
            return;
        }
        
        // Calculate wagons and mules needed
        int availableWagons = townStorage[3].wagons;
        int availableMules = townStorage[3].mules;
        
        int neededWagons = (int)(targetCargo / WAGON_CAPACITY);
        if (neededWagons > availableWagons) neededWagons = availableWagons;
        
        double remainingCargo = targetCargo - (neededWagons * WAGON_CAPACITY);
        int neededMules = (int)(remainingCargo / MULE_CARRY_CAPACITY) + 1;
        if (neededMules > availableMules) neededMules = availableMules;
        
        double actualCapacity = neededWagons * WAGON_CAPACITY + neededMules * MULE_CARRY_CAPACITY;
        if (actualCapacity > targetCargo) actualCapacity = targetCargo;
        
        cargo.wagons = neededWagons;
        cargo.mules = neededMules;
        
        Print("  Preparing convoy: Target %.1f tons, Using %d wagons + %d mules (%.1f tons capacity)\n",
              targetCargo, neededWagons, neededMules, actualCapacity);
        
        // Load supplies proportionally
        double ratio = actualCapacity / availableSupplies;
        if (ratio > 1.0) ratio = 1.0;
        
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            cargo.supplies[i] = townStorage[3].supplies[i] * ratio;
        }
        
        Print("  Cargo prepared: %.1f tons (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n",
              cargo.getTotal(), cargo.supplies[GRAIN], cargo.supplies[FODDER],
              cargo.supplies[SALTED_MEAT], cargo.supplies[WINE_OIL], cargo.supplies[EQUIPMENT]);
        
        // Start convoy from Town 4 to Town 5
        (new Convoy(globalConvoyId, 3, 4, cargo))->Activate();
        
        // Schedule next convoy (every 5-6 hours)
        double nextInterval = Uniform(CONVOY_INTERVAL_T4_MIN, CONVOY_INTERVAL_T4_MAX);
        Activate(Time + nextInterval);
    }
};

// ============================================================================
// GRAPH GENERATION
// ============================================================================

void GenerateCostGraph(int currentDay) {
    const int GRAPH_HEIGHT = COST_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = COST_GRAPH_WIDTH;
    
    // Dynamically scale based on actual maximum cost with 10% padding
    double maxCostInHistory = 0.0;
    for (double cost : campaignCostHistory) {
        if (cost > maxCostInHistory) maxCostInHistory = cost;
    }
    const double MAX_COST = maxCostInHistory * 1.1;  // Add 10% padding at top
    
    costGraphFile.open("campaign_cost_graph.txt");
    
    costGraphFile << "\n";
    costGraphFile << "========================================\n";
    costGraphFile << "  CAMPAIGN COST GRAPH\n";
    costGraphFile << "  Day " << currentDay << " of 100\n";
    costGraphFile << "========================================\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 10, ' '));
    
    // Plot data points
    for (int day = 0; day < (int)campaignCostHistory.size() && day < GRAPH_WIDTH; day++) {
        double cost = campaignCostHistory[day];
        int y = GRAPH_HEIGHT - (int)((cost / MAX_COST) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        int x = day;
        if (x < GRAPH_WIDTH) {
            graph[y][x] = '.';
        }
    }
    
    // Print graph with axes
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double cost = MAX_COST - (y * MAX_COST / GRAPH_HEIGHT);
        costGraphFile << std::to_string((int)cost);
        
        // Pad to align
        int numDigits = std::to_string((int)cost).length();
        for (int i = numDigits; i < 7; i++) {
            costGraphFile << " ";
        }
        
        costGraphFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            costGraphFile << graph[y][x];
        }
        costGraphFile << "\n";
    }
    
    // X-axis
    costGraphFile << "       +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        costGraphFile << "-";
    }
    costGraphFile << "\n";
    
    // X-axis labels
    costGraphFile << "        0";
    for (int i = 10; i <= 100; i += 10) {
        if (i <= GRAPH_WIDTH) {
            int spaces = 10 - std::to_string(i).length();
            for (int j = 0; j < spaces; j++) costGraphFile << " ";
            costGraphFile << i;
        }
    }
    costGraphFile << "\n";
    costGraphFile << "\n        Days ->\n";
    costGraphFile << "\nCost (money) ^\n\n";
    
    // Print data table
    costGraphFile << "Day-by-day costs:\n";
    for (int i = 0; i < (int)campaignCostHistory.size(); i++) {
        costGraphFile << "Day " << i << ": " << campaignCostHistory[i] << " money\n";
    }
    
    costGraphFile.close();
}

void GenerateGraph(int currentDay) {
    const int GRAPH_HEIGHT = SUPPLY_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = SUPPLY_GRAPH_WIDTH;
    const double MAX_SUPPLY = SUPPLY_GRAPH_MAX;  // Maximum expected supply for scaling
    
    graphFile.open("town5_supply_graph.txt");
    
    graphFile << "\n";
    graphFile << "========================================\n";
    graphFile << "  TOWN 5 SUPPLY GRAPH\n";
    graphFile << "  Day " << currentDay << " of 100\n";
    graphFile << "========================================\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 10, ' '));
    
    // Plot data points
    for (int day = 0; day < (int)town5SuppliesHistory.size() && day < GRAPH_WIDTH; day++) {
        double supply = town5SuppliesHistory[day];
        int y = GRAPH_HEIGHT - (int)((supply / MAX_SUPPLY) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        int x = day;
        if (x < GRAPH_WIDTH) {
            graph[y][x] = '.';
        }
    }
    
    // Print graph with axes
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double supply = MAX_SUPPLY - (y * MAX_SUPPLY / GRAPH_HEIGHT);
        graphFile << std::to_string((int)supply);
        
        // Pad to align
        int numDigits = std::to_string((int)supply).length();
        for (int i = numDigits; i < 4; i++) {
            graphFile << " ";
        }
        
        graphFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            graphFile << graph[y][x];
        }
        graphFile << "\n";
    }
    
    // X-axis
    graphFile << "    +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        graphFile << "-";
    }
    graphFile << "\n";
    
    // X-axis labels
    graphFile << "     0";
    for (int i = 10; i <= 100; i += 10) {
        if (i <= GRAPH_WIDTH) {
            int spaces = 10 - std::to_string(i).length();
            for (int j = 0; j < spaces; j++) graphFile << " ";
            graphFile << i;
        }
    }
    graphFile << "\n";
    graphFile << "\n     Days ->\n";
    graphFile << "\nSupplies (tons) ^\n\n";
    
    // Print data table
    graphFile << "Day-by-day data:\n";
    for (int i = 0; i < (int)town5SuppliesHistory.size(); i++) {
        graphFile << "Day " << i << ": " << town5SuppliesHistory[i] << " tons\n";
    }
    
    graphFile.close();
}

void GenerateWeeklyCostGraph(int weekNumber) {
    const int GRAPH_HEIGHT = WEEKLY_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = WEEKLY_GRAPH_WIDTH;  // 7 days in a week
    
    int startIdx = (weekNumber - 1) * 7;
    int endIdx = std::min(startIdx + 7, (int)campaignCostHistory.size());
    
    // Calculate cost at start of this week
    double weekStartCost = (weekNumber == 1) ? 0.0 : campaignCostHistory[startIdx - 1];
    
    // Calculate incremental cost progression within the week (starts at 0)
    std::vector<double> weekIncrementalCosts;
    for (int dayInWeek = 0; dayInWeek < (endIdx - startIdx); dayInWeek++) {
        int absoluteDay = startIdx + dayInWeek;
        double incrementalCost = campaignCostHistory[absoluteDay] - weekStartCost;
        weekIncrementalCosts.push_back(incrementalCost);
    }
    
    // Max cost is the final day of the week
    double maxWeekCost = weekIncrementalCosts.back();
    double totalWeekCost = maxWeekCost;
    if (maxWeekCost == 0) maxWeekCost = 1.0;  // Avoid division by zero
    
    // Open in append mode after first week
    if (weekNumber == 1) {
        weeklyCostGraphFile.open("weekly_cost_graph.txt");
        weeklyCostGraphFile << "\n";
        weeklyCostGraphFile << "========================================\n";
        weeklyCostGraphFile << "  WEEKLY CAMPAIGN COST PROGRESSION\n";
        weeklyCostGraphFile << "  (Shows incremental cost within each week)\n";
        weeklyCostGraphFile << "========================================\n\n";
    } else {
        weeklyCostGraphFile.open("weekly_cost_graph.txt", std::ios::app);
    }
    
    weeklyCostGraphFile << "\n--- WEEK " << weekNumber << " ---\n";
    weeklyCostGraphFile << "Days " << (startIdx) << " to " << (endIdx - 1) << "\n";
    weeklyCostGraphFile << "Total week cost: " << (int)totalWeekCost << " money\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 5, ' '));
    
    // Plot incremental costs for this week (should form ascending line)
    for (int dayInWeek = 0; dayInWeek < (int)weekIncrementalCosts.size(); dayInWeek++) {
        double cost = weekIncrementalCosts[dayInWeek];
        int y = GRAPH_HEIGHT - (int)((cost / maxWeekCost) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        graph[y][dayInWeek] = '.';
    }
    
    // Print graph with axes
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double cost = maxWeekCost - (y * maxWeekCost / GRAPH_HEIGHT);
        std::string costStr = std::to_string((int)cost);
        weeklyCostGraphFile << costStr;
        
        // Pad to align
        for (int i = costStr.length(); i < 7; i++) {
            weeklyCostGraphFile << " ";
        }
        
        weeklyCostGraphFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            weeklyCostGraphFile << graph[y][x];
        }
        weeklyCostGraphFile << "\n";
    }
    
    // X-axis
    weeklyCostGraphFile << "       +-------\n";
    weeklyCostGraphFile << "        M T W T F S S\n";
    
    // Show incremental cost progression
    weeklyCostGraphFile << "\nIncremental cost within this week:\n";
    for (int dayInWeek = 0; dayInWeek < (int)weekIncrementalCosts.size(); dayInWeek++) {
        int absoluteDay = startIdx + dayInWeek;
        weeklyCostGraphFile << "  Day " << absoluteDay << ": " 
                           << (int)weekIncrementalCosts[dayInWeek] << " money (cumulative within week)\n";
    }
    
    weeklyCostGraphFile << "\nTotal Week " << weekNumber << " cost: " 
                       << (int)totalWeekCost << " money\n";
    
    // Store weekly cost for summary graph
    weeklyCosts.push_back(totalWeekCost);
    
    weeklyCostGraphFile.close();
}

void GenerateWeeklySummaryGraph() {
    const int GRAPH_HEIGHT = SUMMARY_GRAPH_HEIGHT;  // Reduced height for better proportions
    const int GRAPH_WIDTH = (int)weeklyCosts.size();
    
    // Fixed Y-axis range from configuration
    const double minCost = SUMMARY_GRAPH_MIN_COST;
    const double maxCost = SUMMARY_GRAPH_MAX_COST;
    const double costRange = maxCost - minCost;
    
    // Append to weekly cost graph file
    weeklyCostGraphFile.open("weekly_cost_graph.txt", std::ios::app);
    
    weeklyCostGraphFile << "\n\n";
    weeklyCostGraphFile << "========================================\n";
    weeklyCostGraphFile << "  WEEKLY COST SUMMARY\n";
    weeklyCostGraphFile << "  (Total cost per week across all weeks)\n";
    weeklyCostGraphFile << "========================================\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 5, ' '));
    
    // Plot weekly costs
    for (int week = 0; week < (int)weeklyCosts.size(); week++) {
        double cost = weeklyCosts[week];
        // Map cost to Y position within fixed range
        int y = GRAPH_HEIGHT - (int)(((cost - minCost) / costRange) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        graph[y][week] = '.';
    }
    
    // Print graph with axes
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double cost = maxCost - (y * costRange / GRAPH_HEIGHT);
        std::string costStr = std::to_string((int)cost);
        weeklyCostGraphFile << costStr;
        
        // Pad to align
        for (int i = costStr.length(); i < 7; i++) {
            weeklyCostGraphFile << " ";
        }
        
        weeklyCostGraphFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            weeklyCostGraphFile << graph[y][x];
        }
        weeklyCostGraphFile << "\n";
    }
    
    // X-axis
    weeklyCostGraphFile << "       +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        weeklyCostGraphFile << "-";
    }
    weeklyCostGraphFile << "\n";
    
    // X-axis labels (week numbers)
    weeklyCostGraphFile << "       ";
    for (int week = 0; week < (int)weeklyCosts.size(); week++) {
        if (week % 5 == 0 || week == (int)weeklyCosts.size() - 1) {
            weeklyCostGraphFile << (week + 1);
        } else {
            weeklyCostGraphFile << " ";
        }
    }
    weeklyCostGraphFile << "\n";
    weeklyCostGraphFile << "\n       Week Number ->\n\n";
    
    // Show all weekly costs
    weeklyCostGraphFile << "Week-by-week costs:\n";
    for (int week = 0; week < (int)weeklyCosts.size(); week++) {
        weeklyCostGraphFile << "  Week " << (week + 1) << ": " 
                           << (int)weeklyCosts[week] << " money\n";
    }
    
    double totalCost = 0.0;
    for (double cost : weeklyCosts) {
        totalCost += cost;
    }
    weeklyCostGraphFile << "\nTotal cost (all complete weeks): " << (int)totalCost << " money\n";
    weeklyCostGraphFile << "Average cost per week: " << (int)(totalCost / weeklyCosts.size()) << " money\n";
    
    weeklyCostGraphFile.close();
}

void GenerateExperimentGraph(const std::string& experimentType) {
    const int GRAPH_HEIGHT = 30;
    const int GRAPH_WIDTH = EXPERIMENT_RUNS;
    
    if (experimentResults.empty()) return;
    
    // Find max cost for scaling
    double maxCost = 0.0;
    for (const auto& result : experimentResults) {
        if (result.totalCost > maxCost) maxCost = result.totalCost;
    }
    maxCost *= 1.1;  // Add 10% padding
    
    std::ofstream expFile("experiment_results.txt");
    
    expFile << "\n";
    expFile << "========================================\n";
    expFile << "  EXPERIMENT RESULTS\n";
    expFile << "  Type: " << experimentType << "\n";
    expFile << "  Runs: " << EXPERIMENT_RUNS << "\n";
    expFile << "========================================\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH * 3 + 10, ' '));
    
    // Plot data points
    for (int run = 0; run < (int)experimentResults.size(); run++) {
        double cost = experimentResults[run].totalCost;
        int y = GRAPH_HEIGHT - (int)((cost / maxCost) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        int x = run * 3;
        graph[y][x] = '.';
    }
    
    // Print graph with axes
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double cost = maxCost - (y * maxCost / GRAPH_HEIGHT);
        std::string costStr = std::to_string((int)cost);
        expFile << costStr;
        
        for (int i = costStr.length(); i < 8; i++) {
            expFile << " ";
        }
        
        expFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH * 3; x++) {
            expFile << graph[y][x];
        }
        expFile << "\n";
    }
    
    // X-axis
    expFile << "        +";
    for (int i = 0; i < GRAPH_WIDTH * 3; i++) {
        expFile << "-";
    }
    expFile << "\n";
    
    // X-axis labels (run numbers)
    expFile << "         ";
    for (int run = 0; run < EXPERIMENT_RUNS; run++) {
        expFile << (run + 1);
        if (run < EXPERIMENT_RUNS - 1) expFile << "  ";
    }
    expFile << "\n";
    expFile << "\n         Run Number ->\n";
    expFile << "\nCost (money) ^\n\n";
    
    // Print detailed results table
    expFile << "Detailed Results:\n";
    expFile << "Run | Parameter Value | Total Cost | Final Supplies | Convoys\n";
    expFile << "----+----------------+------------+----------------+--------\n";
    for (int run = 0; run < (int)experimentResults.size(); run++) {
        expFile << std::setw(3) << (run + 1) << " | ";
        expFile << std::setw(14) << std::fixed << std::setprecision(2) << experimentResults[run].parameterValue << " | ";
        expFile << std::setw(10) << (int)experimentResults[run].totalCost << " | ";
        expFile << std::setw(14) << std::fixed << std::setprecision(1) << experimentResults[run].finalSupplies << " | ";
        expFile << std::setw(6) << experimentResults[run].convoyCount << "\n";
    }
    
    expFile << "\n";
    expFile << "Summary Statistics:\n";
    double avgCost = 0.0;
    for (const auto& result : experimentResults) {
        avgCost += result.totalCost;
    }
    avgCost /= experimentResults.size();
    expFile << "Average cost across all runs: " << (int)avgCost << " money\n";
    expFile << "Minimum cost: " << (int)experimentResults[0].totalCost << " money (Run 1)\n";
    expFile << "Maximum cost: " << (int)experimentResults[experimentResults.size()-1].totalCost << " money (Run " << experimentResults.size() << ")\n";
    
    expFile.close();
}

// ============================================================================
// DAILY CONSUMPTION FOR TOWN 5
// ============================================================================

class DailyConsumptionProcess : public Process {
    void Behavior() {
        // Record initial supply and cost at day 0
        town5SuppliesHistory.push_back(townStorage[4].getTotal());
        campaignCostHistory.push_back(campaignCost);
        GenerateGraph(0);
        GenerateCostGraph(0);
        
        while (true) {
            Wait(1.0);  // Wait 1 day
            
            int currentDay = (int)Time;
            
            // Consume supplies from Town 5 (last town)
            Seize(*towns[4]);
            
            double availableSupplies = townStorage[4].getTotal();
            double consumptionToday = CONSUMPTION_PER_DAY_ACTIVE;
            
            if (availableSupplies < consumptionToday) {
                consumptionToday = availableSupplies;
            }
            
            if (consumptionToday > 0) {
                // Consume supplies proportionally
                double ratio = consumptionToday / availableSupplies;
                
                for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                    double consumed = townStorage[4].supplies[i] * ratio;
                    townStorage[4].supplies[i] -= consumed;
                }
                
                Print("\n[DAY %d] Town 5 consumed %.1f tons (%.1f tons remaining)\n", 
                      currentDay, consumptionToday, townStorage[4].getTotal());
            } else {
                Print("\n[DAY %d] Town 5 has NO SUPPLIES - starvation!\n", currentDay);
            }
            
            Release(*towns[4]);
            
            // Record supply and cost after consumption and update graphs
            town5SuppliesHistory.push_back(townStorage[4].getTotal());
            campaignCostHistory.push_back(campaignCost);
            GenerateGraph(currentDay);
            GenerateCostGraph(currentDay);
            
            // Generate weekly cost graph at end of each week
            if (currentDay % 7 == 0 && currentDay > 0) {
                int weekNumber = currentDay / 7;
                GenerateWeeklyCostGraph(weekNumber);
            }
        }
    }
};

// ============================================================================
// SIMULATION RUNNER
// ============================================================================
// ============================================================================
// MAIN
// ============================================================================

int main() {
    SetOutput("phase2_output.txt");
    
    InitializeFacilities();
    
    if (EXPERIMENT_MODE) {
        Print("========================================\n");
        Print("  EXPERIMENT MODE\n");
        Print("========================================\n\n");
        
        std::string experimentType = "";
        double baseValue = 0.0;
        
        if (EXP_WEATHER_DURATION) {
            experimentType = "Weather Duration (exponential)";
            baseValue = 2.0;  // base duration
            Print("Running %d simulations with exponentially increasing weather duration\n", EXPERIMENT_RUNS);
            Print("Base duration: %.2f-%.2f days\n", 2.0, 5.0);
            Print("Multiplier: %.2f\n\n", EXPERIMENT_MULTIPLIER);
            
            for (int run = 0; run < EXPERIMENT_RUNS; run++) {
                double multiplier = std::pow(EXPERIMENT_MULTIPLIER, run);
                WEATHER_DURATION_MIN_ACTIVE = 2.0 * multiplier;
                WEATHER_DURATION_MAX_ACTIVE = 5.0 * multiplier;
                
                Print("\n=== RUN %d/%d ===\n", run + 1, EXPERIMENT_RUNS);
                Print("Weather duration: %.2f-%.2f days\n", WEATHER_DURATION_MIN_ACTIVE, WEATHER_DURATION_MAX_ACTIVE);
                
                // Reset and run simulation
                campaignCost = 0.0;
                globalConvoyId = 0;
                town5SuppliesHistory.clear();
                campaignCostHistory.clear();
                weeklyCosts.clear();
                weatherDelayMultiplier = 1.0;
                currentWeatherDelayPercentage = 0.0;
                isWeatherActive = false;
                totalRobberAttacks = 0;
                totalSuppliesStolen = 0.0;
                for (int i = 0; i < 4; i++) RobberAttacksPerRoute[i].Clear();
                ConvoyCountTown1.Clear();
                ConvoyCountTown4.Clear();
                
                InitializeTownSupplies();
                Init(0, SIMULATION_DAYS);
                
                (new Town1ConvoyGenerator)->Activate();
                (new Town4ConvoyGenerator)->Activate();
                (new DailyConsumptionProcess)->Activate();
                
                if (WEATHER_ENABLED) {
                    (new BadWeatherEvent)->Activate(Time + Uniform(WEATHER_INITIAL_DELAY_MIN, WEATHER_INITIAL_DELAY_MAX));
                }
                
                Run();
                
                // Store results
                ExperimentResult result;
                result.parameterValue = WEATHER_DURATION_MIN_ACTIVE;
                result.totalCost = campaignCost;
                result.finalSupplies = townStorage[4].getTotal();
                result.convoyCount = (int)ConvoyCountTown1.Number();
                experimentResults.push_back(result);
                
                Print("Run %d complete - Cost: %.2f, Final supplies: %.2f\n", 
                      run + 1, campaignCost, townStorage[4].getTotal());
            }
            
        } else if (EXP_WEATHER_INTERVAL) {
            experimentType = "Weather Interval (exponential)";
            baseValue = 7.0;  // base interval
            Print("Running %d simulations with exponentially increasing weather interval\n", EXPERIMENT_RUNS);
            Print("Base interval: %.2f-%.2f days\n", 7.0, 14.0);
            Print("Multiplier: %.2f\n\n", EXPERIMENT_MULTIPLIER);
            
            for (int run = 0; run < EXPERIMENT_RUNS; run++) {
                double multiplier = std::pow(EXPERIMENT_MULTIPLIER, run);
                WEATHER_INTERVAL_MIN_ACTIVE = 7.0 * multiplier;
                WEATHER_INTERVAL_MAX_ACTIVE = 14.0 * multiplier;
                
                Print("\n=== RUN %d/%d ===\n", run + 1, EXPERIMENT_RUNS);
                Print("Weather interval: %.2f-%.2f days\n", WEATHER_INTERVAL_MIN_ACTIVE, WEATHER_INTERVAL_MAX_ACTIVE);
                
                // Reset and run simulation
                campaignCost = 0.0;
                globalConvoyId = 0;
                town5SuppliesHistory.clear();
                campaignCostHistory.clear();
                weeklyCosts.clear();
                weatherDelayMultiplier = 1.0;
                currentWeatherDelayPercentage = 0.0;
                isWeatherActive = false;
                totalRobberAttacks = 0;
                totalSuppliesStolen = 0.0;
                for (int i = 0; i < 4; i++) RobberAttacksPerRoute[i].Clear();
                ConvoyCountTown1.Clear();
                ConvoyCountTown4.Clear();
                
                InitializeTownSupplies();
                Init(0, SIMULATION_DAYS);
                
                (new Town1ConvoyGenerator)->Activate();
                (new Town4ConvoyGenerator)->Activate();
                (new DailyConsumptionProcess)->Activate();
                
                if (WEATHER_ENABLED) {
                    (new BadWeatherEvent)->Activate(Time + Uniform(WEATHER_INITIAL_DELAY_MIN, WEATHER_INITIAL_DELAY_MAX));
                }
                
                Run();
                
                // Store results
                ExperimentResult result;
                result.parameterValue = WEATHER_INTERVAL_MIN_ACTIVE;
                result.totalCost = campaignCost;
                result.finalSupplies = townStorage[4].getTotal();
                result.convoyCount = (int)ConvoyCountTown1.Number();
                experimentResults.push_back(result);
                
                Print("Run %d complete - Cost: %.2f, Final supplies: %.2f\n", 
                      run + 1, campaignCost, townStorage[4].getTotal());
            }
            
        } else if (EXP_CONSUMPTION) {
            experimentType = "Consumption Rate (exponential)";
            baseValue = 30.0;  // base consumption
            Print("Running %d simulations with exponentially increasing consumption rate\n", EXPERIMENT_RUNS);
            Print("Base consumption: %.2f tons/day\n", 30.0);
            Print("Multiplier: %.2f\n\n", EXPERIMENT_MULTIPLIER);
            
            for (int run = 0; run < EXPERIMENT_RUNS; run++) {
                double multiplier = std::pow(EXPERIMENT_MULTIPLIER, run);
                CONSUMPTION_PER_DAY_ACTIVE = 30.0 * multiplier;
                
                Print("\n=== RUN %d/%d ===\n", run + 1, EXPERIMENT_RUNS);
                Print("Consumption rate: %.2f tons/day\n", CONSUMPTION_PER_DAY_ACTIVE);
                
                // Reset and run simulation
                campaignCost = 0.0;
                globalConvoyId = 0;
                town5SuppliesHistory.clear();
                campaignCostHistory.clear();
                weeklyCosts.clear();
                weatherDelayMultiplier = 1.0;
                currentWeatherDelayPercentage = 0.0;
                isWeatherActive = false;
                totalRobberAttacks = 0;
                totalSuppliesStolen = 0.0;
                for (int i = 0; i < 4; i++) RobberAttacksPerRoute[i].Clear();
                ConvoyCountTown1.Clear();
                ConvoyCountTown4.Clear();
                
                InitializeTownSupplies();
                Init(0, SIMULATION_DAYS);
                
                (new Town1ConvoyGenerator)->Activate();
                (new Town4ConvoyGenerator)->Activate();
                (new DailyConsumptionProcess)->Activate();
                
                if (WEATHER_ENABLED) {
                    (new BadWeatherEvent)->Activate(Time + Uniform(WEATHER_INITIAL_DELAY_MIN, WEATHER_INITIAL_DELAY_MAX));
                }
                
                Run();
                
                // Store results
                ExperimentResult result;
                result.parameterValue = CONSUMPTION_PER_DAY_ACTIVE;
                result.totalCost = campaignCost;
                result.finalSupplies = townStorage[4].getTotal();
                result.convoyCount = (int)ConvoyCountTown1.Number();
                experimentResults.push_back(result);
                
                Print("Run %d complete - Cost: %.2f, Final supplies: %.2f\n", 
                      run + 1, campaignCost, townStorage[4].getTotal());
            }
        }
        
        // Generate comparison graph
        GenerateExperimentGraph(experimentType);
        
        Print("\n========================================\n");
        Print("  EXPERIMENT COMPLETED\n");
        Print("========================================\n");
        Print("Results saved to: experiment_results.txt\n");
        
    } else {
        // Original single-run mode
        InitializeTownSupplies();
        
        Print("========================================\n");
        Print("  SUPPLY CONVOY SIMULATION - 5 TOWNS\n");
        Print("========================================\n\n");
    
    Print("Configuration:\n");
    Print("  Number of towns: %d\n", NUM_TOWNS);
    Print("  Town 1->2 convoys per week: %d (every %.2f days)\n", 
          CONVOYS_PER_WEEK_T1, CONVOY_INTERVAL_T1);
    Print("  Town 1->2 convoy composition: %d wagons + %d mules\n",
          WAGONS_PER_CONVOY_T1, MULES_PER_CONVOY_T1);
    Print("  Wagon capacity: %.2f tons\n", WAGON_CAPACITY);
    Print("  Mule capacity: %.2f tons\n", MULE_CARRY_CAPACITY);
    Print("  Town 4->5 convoy interval: %.2f-%.2f hours (%.4f-%.4f days)\n",
          CONVOY_INTERVAL_T4_MIN * 24, CONVOY_INTERVAL_T4_MAX * 24,
          CONVOY_INTERVAL_T4_MIN, CONVOY_INTERVAL_T4_MAX);
    Print("  Town 4->5 convoy cargo: %.1f-%.1f tons\n\n", MIN_CARGO_T4, MAX_CARGO_T4);
    
    Print("Initial town supplies:\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        Print("  Town %d: %.1f tons total", i + 1, townStorage[i].getTotal());
        if (i == 0) {
            Print(" (unlimited)\n");
        } else if (i == 4) {
            Print(" (starting buffer - G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f)\n",
                  townStorage[i].supplies[GRAIN], townStorage[i].supplies[FODDER],
                  townStorage[i].supplies[SALTED_MEAT], townStorage[i].supplies[WINE_OIL],
                  townStorage[i].supplies[EQUIPMENT]);
        } else {
            Print(" (G:%.1f F:%.1f SM:%.1f WO:%.1f Eq:%.1f, %d wagons, %d mules)\n",
                  townStorage[i].supplies[GRAIN], townStorage[i].supplies[FODDER],
                  townStorage[i].supplies[SALTED_MEAT], townStorage[i].supplies[WINE_OIL],
                  townStorage[i].supplies[EQUIPMENT], townStorage[i].wagons, townStorage[i].mules);
        }
    }
    Print("\n");
    
        Print("Starting simulation for %d days...\n", SIMULATION_DAYS);
        Print("Town 5 consumption rate: %.1f tons per day (%.1f tons per week)\n", 
              CONSUMPTION_PER_DAY_ACTIVE, CONSUMPTION_PER_WEEK);
        Print("========================================\n");    Init(0, SIMULATION_DAYS);  // Initialize simulation
    
    // Start convoy generators and daily consumption AFTER Init()
    (new Town1ConvoyGenerator)->Activate();
    (new Town4ConvoyGenerator)->Activate();  // Both start at time 0
    (new DailyConsumptionProcess)->Activate();  // Start daily consumption
    
    // Start weather events (if enabled)
    if (WEATHER_ENABLED) {
        (new BadWeatherEvent)->Activate(Time + Uniform(WEATHER_INITIAL_DELAY_MIN, WEATHER_INITIAL_DELAY_MAX));
        Print("Weather system: ENABLED\n");
    } else {
        Print("Weather system: DISABLED\n");
    }
    
        Run();
        
        // Generate weekly summary graph at the end
        GenerateWeeklySummaryGraph();
        
        Print("\n========================================\n");
        Print("  SIMULATION COMPLETED\n");
        Print("========================================\n\n");
        
        Print("Final town supplies:\n");
        for (int i = 0; i < NUM_TOWNS; i++) {
            Print("  Town %d: %.1f tons total\n", i + 1, townStorage[i].getTotal());
            Print("    Grain: %.1f tons, Fodder: %.1f tons, Salted Meat: %.1f tons\n",
                  townStorage[i].supplies[GRAIN], townStorage[i].supplies[FODDER],
                  townStorage[i].supplies[SALTED_MEAT]);
            Print("    Wine/Oil: %.1f tons, Equipment: %.1f tons\n",
                  townStorage[i].supplies[WINE_OIL], townStorage[i].supplies[EQUIPMENT]);
            Print("    Wagons: %d, Mules: %d\n\n", townStorage[i].wagons, townStorage[i].mules);
        }
        
        ConvoyCountTown1.Output();
        ConvoyCountTown4.Output();
        TotalDeliveredToTown5.Output();
        
        if (ROBBERS_ENABLED) {
            Print("\n========================================\n");
            Print("  ROBBER ATTACK STATISTICS\n");
            Print("========================================\n");
            Print("Total robber attacks: %d\n", totalRobberAttacks);
            Print("Total supplies stolen: %.2f tons\n", totalSuppliesStolen);
            Print("\nAttacks per route:\n");
            for (int i = 0; i < 4; i++) {
                RobberAttacksPerRoute[i].Output();
            }
        }
        
        Print("\n========================================\n");
        Print("  CAMPAIGN ECONOMICS\n");
        Print("========================================\n");
        Print("Total campaign cost: %.2f money\n", campaignCost);
        Print("Average cost per day: %.2f money\n", campaignCost / (double)SIMULATION_DAYS);
        Print("\nTown 5 supply graph saved to: town5_supply_graph.txt\n");
        Print("Campaign cost graph saved to: campaign_cost_graph.txt\n");
        Print("Weekly cost progression saved to: weekly_cost_graph.txt\n");
    }
    
    return 0;
}
