#include <simlib.h>
#include <vector>
#include <string>
#include <map>
#include <cstdarg>

// ============================================================================
// OUTPUT FILE HANDLES
// ============================================================================

FILE* mainLog = nullptr;           // Main simulation log
FILE* convoyDetails = nullptr;     // Convoy operations details
FILE* townStats = nullptr;         // Town supply tracking
FILE* resourceFlow = nullptr;      // Resource movement analysis
FILE* summaryFile = nullptr;       // Executive summary

// Helper to print to multiple files
void PrintToMain(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(mainLog, format, args);
    va_end(args);
    fflush(mainLog);
}

void PrintToConvoy(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(convoyDetails, format, args);
    va_end(args);
    fflush(convoyDetails);
}

void PrintToTown(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(townStats, format, args);
    va_end(args);
    fflush(townStats);
}

void PrintToFlow(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(resourceFlow, format, args);
    va_end(args);
    fflush(resourceFlow);
}

void PrintToSummary(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(summaryFile, format, args);
    va_end(args);
    fflush(summaryFile);
}

// ============================================================================
// SUPPLY TYPES
// ============================================================================

enum SupplyType {
    WHEAT,
    BARLEY,
    MULES,  // Transport animals only (not cargo)
    ANIMAL_FODDER,
    WEAPONS,
    CLOTHING,
    PEOPLE,
    NUM_SUPPLY_TYPES
};

const char* SupplyNames[] = {
    "Wheat",
    "Barley",
    "Mules",
    "Animal Fodder",
    "Weapons",
    "Clothing",
    "People"
};

// ============================================================================
// CONFIGURATION
// ============================================================================

// Travel times between consecutive towns (in days)
double townTimes[] = {5.0, 7.0, 6.0, 8.0, 6.0, 7.0, 5.0};

// Number of towns (one more than number of routes)
const int NUM_TOWNS = sizeof(townTimes) / sizeof(townTimes[0]) + 1;

// Maximum capacity for each convoy route (tons per supply type)
double maxConvoyCapacity[] = {50.0, 60.0, 45.0, 70.0, 55.0, 50.0, 65.0};

// Wagon configuration
const double WAGON_CAPACITY = 5.0;  // tons per wagon
const int MULES_PER_WAGON = 2;      // mules needed to pull one wagon
const double MULE_CARRY_CAPACITY = 0.15;  // tons per mule when carrying on back
const double WAGON_SPEED_PENALTY = 1.3;  // Wagons are 30% slower than mules on backs

// ============================================================================
// TOWN SUPPLY STORAGE
// ============================================================================

struct TownSupplies {
    double supplies[NUM_SUPPLY_TYPES];
    int wagons;  // Number of wagons in this supply set
    
    TownSupplies() : wagons(0) {
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            supplies[i] = 0.0;
        }
    }
    
    void add(const TownSupplies& other) {
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            supplies[i] += other.supplies[i];
        }
        wagons += other.wagons;
    }
    
    double getTotal() const {
        double total = 0.0;
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            // Don't count people and mules in tonnage
            if (i != PEOPLE && i != MULES) {
                total += supplies[i];
            }
        }
        return total;
    }
    
    void print(const char* prefix) const {
        Print("%s", prefix);
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            if (i == PEOPLE) {
                Print("%s: %d people", SupplyNames[i], (int)supplies[i]);
            } else if (i == MULES) {
                Print("%s: %d mules", SupplyNames[i], (int)supplies[i]);
            } else {
                Print("%s: %.1f tons", SupplyNames[i], supplies[i]);
            }
            if (i < NUM_SUPPLY_TYPES - 1) Print(", ");
        }
        Print(", Wagons: %d", wagons);
        Print("\n");
    }
};

// Global town storage
std::vector<TownSupplies> townStorage;
std::vector<Facility*> towns;

// ============================================================================
// INITIAL SUPPLY CONFIGURATION
// ============================================================================

void InitializeTownSupplies() {
    townStorage.resize(NUM_TOWNS);
    
    // Town 1: Initial supplies
    townStorage[0].supplies[WHEAT] = 100.0;
    townStorage[0].supplies[BARLEY] = 80.0;
    townStorage[0].supplies[MULES] = 50.0;  // 50 mules (transport only)
    townStorage[0].supplies[ANIMAL_FODDER] = 120.0;
    townStorage[0].supplies[WEAPONS] = 30.0;
    townStorage[0].supplies[CLOTHING] = 60.0;
    townStorage[0].supplies[PEOPLE] = 20.0;  // 20 people
    townStorage[0].wagons = 5;  // Initial wagon stock
    
    // Town 2
    townStorage[1].supplies[WHEAT] = 50.0;
    townStorage[1].supplies[BARLEY] = 40.0;
    townStorage[1].supplies[MULES] = 20.0;  // 20 mules
    townStorage[1].supplies[ANIMAL_FODDER] = 60.0;
    townStorage[1].supplies[WEAPONS] = 15.0;
    townStorage[1].supplies[CLOTHING] = 30.0;
    townStorage[1].supplies[PEOPLE] = 10.0;  // 10 people
    townStorage[1].wagons = 3;  // Initial wagon stock
    
    // Town 3
    townStorage[2].supplies[WHEAT] = 70.0;
    townStorage[2].supplies[BARLEY] = 60.0;
    townStorage[2].supplies[MULES] = 30.0;  // 30 mules
    townStorage[2].supplies[ANIMAL_FODDER] = 80.0;
    townStorage[2].supplies[WEAPONS] = 20.0;
    townStorage[2].supplies[CLOTHING] = 40.0;
    townStorage[2].supplies[PEOPLE] = 15.0;  // 15 people
    townStorage[2].wagons = 4;  // Initial wagon stock
    
    // Town 4
    townStorage[3].supplies[WHEAT] = 40.0;
    townStorage[3].supplies[BARLEY] = 30.0;
    townStorage[3].supplies[MULES] = 15.0;  // 15 mules
    townStorage[3].supplies[ANIMAL_FODDER] = 50.0;
    townStorage[3].supplies[WEAPONS] = 10.0;
    townStorage[3].supplies[CLOTHING] = 25.0;
    townStorage[3].supplies[PEOPLE] = 8.0;  // 8 people
    townStorage[3].wagons = 2;  // Initial wagon stock
    
    // Town 5
    townStorage[4].supplies[WHEAT] = 60.0;
    townStorage[4].supplies[BARLEY] = 50.0;
    townStorage[4].supplies[MULES] = 25.0;  // 25 mules
    townStorage[4].supplies[ANIMAL_FODDER] = 70.0;
    townStorage[4].supplies[WEAPONS] = 18.0;
    townStorage[4].supplies[CLOTHING] = 35.0;
    townStorage[4].supplies[PEOPLE] = 12.0;  // 12 people
    townStorage[4].wagons = 4;  // Initial wagon stock
    
    // Town 6
    townStorage[5].supplies[WHEAT] = 30.0;
    townStorage[5].supplies[BARLEY] = 25.0;
    townStorage[5].supplies[MULES] = 10.0;  // 10 mules
    townStorage[5].supplies[ANIMAL_FODDER] = 40.0;
    townStorage[5].supplies[WEAPONS] = 8.0;
    townStorage[5].supplies[CLOTHING] = 20.0;
    townStorage[5].supplies[PEOPLE] = 6.0;  // 6 people
    townStorage[5].wagons = 2;  // Initial wagon stock
    
    // Town 7
    townStorage[6].supplies[WHEAT] = 45.0;
    townStorage[6].supplies[BARLEY] = 35.0;
    townStorage[6].supplies[MULES] = 18.0;  // 18 mules
    townStorage[6].supplies[ANIMAL_FODDER] = 55.0;
    townStorage[6].supplies[WEAPONS] = 12.0;
    townStorage[6].supplies[CLOTHING] = 28.0;
    townStorage[6].supplies[PEOPLE] = 9.0;  // 9 people
    townStorage[6].wagons = 3;  // Initial wagon stock
    
    // Town 8: Destination (empty initially)
    // All zeros by default
}

void InitializeFacilities() {
    for (int i = 0; i < NUM_TOWNS; i++) {
        std::string name = "Town " + std::to_string(i + 1);
        towns.push_back(new Facility(name.c_str()));
    }
}

// Statistics
Stat ConvoyCount("Number of convoys");
Stat TotalSuppliesDelivered("Total supplies delivered to final town (tons)");

// ============================================================================
// GLOBAL WEATHER STATE
// ============================================================================

double weatherDelayMultiplier = 1.0;
double currentWeatherDelayPercentage = 0.0;
bool isWeatherActive = false;

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
    currentWeatherDelayPercentage = Uniform(10, 50);
    weatherDelayMultiplier = 1.0 + (currentWeatherDelayPercentage / 100.0);
    isWeatherActive = true;
    
    Print("[TIME %.2f] *** BAD WEATHER STARTS ***\n", Time);
    Print("              Delay: +%.1f%% | Multiplier: %.2fx\n\n", 
          currentWeatherDelayPercentage, weatherDelayMultiplier);
    
    (new WeatherClearEvent)->Activate(Time + Uniform(2, 5));
}

void WeatherClearEvent::Behavior() {
    weatherDelayMultiplier = 1.0;
    currentWeatherDelayPercentage = 0.0;
    isWeatherActive = false;
    
    Print("[TIME %.2f] >>> WEATHER CLEARED <<<\n", Time);
    Print("              Travel times back to normal\n\n");
    
    (new BadWeatherEvent)->Activate(Time + Uniform(10, 20));
}

// ============================================================================
// DAILY TOWN CONSUMPTION EVENT
// ============================================================================

class DailyConsumptionProcess : public Process {
    void Behavior();
};

void DailyConsumptionProcess::Behavior() {
    while (true) {
        // Wait until next day
        Wait(1.0);
        
        // Each town consumes supplies daily
        int totalAnimalDeaths = 0;
        int totalPeopleDeaths = 0;
        
        for (int i = 0; i < NUM_TOWNS; i++) {
            // Seize town to prevent concurrent access during consumption
            Seize(*towns[i]);
            
            int townPeople = (int)townStorage[i].supplies[PEOPLE];
            int townMules = (int)townStorage[i].supplies[MULES];
            
            // People consume food (wheat/barley)
            double peopleFood = townPeople * 0.02; // tons per person per day
            double foodConsumed = 0.0;
            
            if (townStorage[i].supplies[WHEAT] >= peopleFood) {
                townStorage[i].supplies[WHEAT] -= peopleFood;
                foodConsumed = peopleFood;
            } else {
                foodConsumed = townStorage[i].supplies[WHEAT];
                double remaining = peopleFood - townStorage[i].supplies[WHEAT];
                townStorage[i].supplies[WHEAT] = 0.0;
                if (townStorage[i].supplies[BARLEY] >= remaining) {
                    townStorage[i].supplies[BARLEY] -= remaining;
                    foodConsumed += remaining;
                } else {
                    foodConsumed += townStorage[i].supplies[BARLEY];
                    townStorage[i].supplies[BARLEY] = 0.0;
                }
            }
            
            // People starvation - if not enough food
            if (foodConsumed < peopleFood && townPeople > 0) {
                double foodShortage = peopleFood - foodConsumed;
                double starvationRate = 0.08 * (foodShortage / peopleFood); // 8% base rate
                int peopleDeaths = (int)(townPeople * starvationRate);
                if (peopleDeaths > townPeople) peopleDeaths = townPeople;
                townStorage[i].supplies[PEOPLE] -= peopleDeaths;
                totalPeopleDeaths += peopleDeaths;
            }
            
            // Mules consume fodder (town mules consume less than traveling mules)
            double muleFodder = townMules * 0.02; // tons per mule per day (stationary)
            if (townStorage[i].supplies[ANIMAL_FODDER] >= muleFodder) {
                townStorage[i].supplies[ANIMAL_FODDER] -= muleFodder;
            } else {
                // Not enough fodder - mules may die
                townStorage[i].supplies[ANIMAL_FODDER] = 0.0;
                double lossRate = 0.05; // 5% die per day without food
                int muleDeaths = (int)(townMules * lossRate);
                if (muleDeaths > townMules) muleDeaths = townMules;
                townStorage[i].supplies[MULES] -= muleDeaths;
                totalAnimalDeaths += muleDeaths;
            }
            
            // Release town after consumption complete
            Release(*towns[i]);
        }
        
        // Print daily report showing all town supplies
        int currentDay = (int)Time;
        PrintToTown("\n[DAY %d]\n", currentDay);
        
        if (totalAnimalDeaths > 0 || totalPeopleDeaths > 0) {
            PrintToMain("\n[DAY %d] Deaths: ", currentDay);
            if (totalPeopleDeaths > 0) {
                PrintToMain("%d people", totalPeopleDeaths);
            }
            if (totalPeopleDeaths > 0 && totalAnimalDeaths > 0) {
                PrintToMain(", ");
            }
            if (totalAnimalDeaths > 0) {
                PrintToMain("%d mules", totalAnimalDeaths);
            }
            PrintToMain("\n");
        }
        
        // Show supplies for all towns
        for (int i = 0; i < NUM_TOWNS; i++) {
            PrintToTown("Town %d: %.1f tons - W:%.0f B:%.0f M:%d F:%.0f Wp:%.0f C:%.0f P:%d Wg:%d\n",
                  i + 1, townStorage[i].getTotal(),
                  townStorage[i].supplies[WHEAT],
                  townStorage[i].supplies[BARLEY],
                  (int)townStorage[i].supplies[MULES],
                  townStorage[i].supplies[ANIMAL_FODDER],
                  townStorage[i].supplies[WEAPONS],
                  townStorage[i].supplies[CLOTHING],
                  (int)townStorage[i].supplies[PEOPLE],
                  townStorage[i].wagons);
        }
    }
}

// ============================================================================
// CONVOY PROCESS - Transports supplies from one town to next
// ============================================================================

class Convoy : public Process {
    int fromTown;  // Index of origin town
    int toTown;    // Index of destination town
    int convoyId;  // Unique ID for this convoy
    TownSupplies cargo;
    
    void Behavior() {
        PrintToMain("\n[TIME %.2f] Convoy #%d: Starting from Town %d to Town %d\n", 
              Time, convoyId, fromTown + 1, toTown + 1);
        PrintToConvoy("\n[TIME %.2f] Convoy #%d: Departure\n", Time, convoyId);
        PrintToConvoy("  Route: Town %d -> Town %d\n", fromTown + 1, toTown + 1);
        
        // Load supplies from origin town
        Seize(*towns[fromTown]);
        
        PrintToConvoy("  Loading supplies from Town %d...\n", fromTown + 1);
        
        // SMART TRANSPORT ALGORITHM
        // Decision: Mules on backs (fast, low capacity) vs Wagons (slow, high capacity) vs Mixed
        
        double maxCapacity = maxConvoyCapacity[fromTown];
        double totalAvailable = townStorage[fromTown].getTotal();
        int availableMules = (int)townStorage[fromTown].supplies[MULES];
        int availableWagons = townStorage[fromTown].wagons;
        
        // Calculate all options
        double mulesOnlyCapacity = availableMules * MULE_CARRY_CAPACITY;
        int maxPossibleWagons = (availableWagons < availableMules / MULES_PER_WAGON) ? 
                                 availableWagons : availableMules / MULES_PER_WAGON;
        
        // Option 1: All mules carry on backs (FAST)
        double option1_capacity = (mulesOnlyCapacity < maxCapacity) ? mulesOnlyCapacity : maxCapacity;
        double option1_speed = 1.0;  // Base speed
        
        // Option 2: Maximum wagons (SLOW but HIGH capacity)
        int option2_wagons = maxPossibleWagons;
        int option2_mulesInWagons = option2_wagons * MULES_PER_WAGON;
        int option2_mulesCarrying = availableMules - option2_mulesInWagons;
        double option2_capacity = option2_wagons * WAGON_CAPACITY + option2_mulesCarrying * MULE_CARRY_CAPACITY;
        if (option2_capacity > maxCapacity) option2_capacity = maxCapacity;
        double option2_speed = WAGON_SPEED_PENALTY;  // Slower
        
        // Option 3: MIXED - Use wagons only if supply amount justifies the speed penalty
        // Strategy: Use wagons if we have more supplies than mules-only can handle
        int option3_wagons = 0;
        int option3_mulesInWagons = 0;
        int option3_mulesCarrying = availableMules;
        double option3_capacity = mulesOnlyCapacity;
        double option3_speed = 1.0;
        
        if (totalAvailable > mulesOnlyCapacity && availableWagons > 0) {
            // Need wagons for the extra capacity
            double excessLoad = totalAvailable - mulesOnlyCapacity;
            option3_wagons = (int)(excessLoad / WAGON_CAPACITY) + 1;
            if (option3_wagons > maxPossibleWagons) option3_wagons = maxPossibleWagons;
            option3_mulesInWagons = option3_wagons * MULES_PER_WAGON;
            option3_mulesCarrying = availableMules - option3_mulesInWagons;
            option3_capacity = option3_wagons * WAGON_CAPACITY + option3_mulesCarrying * MULE_CARRY_CAPACITY;
            if (option3_capacity > maxCapacity) option3_capacity = maxCapacity;
            option3_speed = WAGON_SPEED_PENALTY;
        }
        
        // DECISION: Choose best option based on supply amount
        int chosenWagons = 0;
        int mulesInWagons = 0;
        int mulesCarrying = 0;
        int totalMules = 0;  // Total mules traveling with convoy
        double finalCapacity = 0;
        double speedMultiplier = 1.0;
        const char* transportMode = "Unknown";
        
        if (totalAvailable <= option1_capacity) {
            // Option 1: Mules only (enough capacity, faster)
            chosenWagons = 0;
            mulesInWagons = 0;
            mulesCarrying = availableMules;
            totalMules = availableMules;
            finalCapacity = option1_capacity;
            speedMultiplier = option1_speed;
            transportMode = "MULES ON BACKS (fast)";
        } else if (totalAvailable <= option3_capacity && option3_wagons > 0) {
            // Option 3: Mixed mode (optimal for this load)
            chosenWagons = option3_wagons;
            mulesInWagons = option3_mulesInWagons;
            mulesCarrying = option3_mulesCarrying;
            totalMules = mulesInWagons + mulesCarrying;
            finalCapacity = option3_capacity;
            speedMultiplier = option3_speed;
            transportMode = "MIXED MODE (wagons + mules)";
        } else {
            // Option 2: Maximum wagons (need all capacity)
            chosenWagons = option2_wagons;
            mulesInWagons = option2_mulesInWagons;
            mulesCarrying = option2_mulesCarrying;
            totalMules = mulesInWagons + mulesCarrying;
            finalCapacity = option2_capacity;
            speedMultiplier = option2_speed;
            transportMode = "FULL WAGONS (high capacity, slow)";
        }
        
        PrintToConvoy("  Transport Mode: %s\n", transportMode);
        if (chosenWagons > 0) {
            PrintToConvoy("  Composition: %d wagons (pulled by %d mules) + %d mules carrying\n",
                  chosenWagons, mulesInWagons, mulesCarrying);
            PrintToConvoy("  Capacity: %.1f tons (%.1f from wagons + %.1f from mules)\n",
                  finalCapacity, chosenWagons * WAGON_CAPACITY, mulesCarrying * MULE_CARRY_CAPACITY);
        } else {
            PrintToConvoy("  Composition: %d mules carrying on backs\n", mulesCarrying);
            PrintToConvoy("  Capacity: %.1f tons\n", finalCapacity);
        }
        PrintToConvoy("  Speed: %.0f%% of base speed\n", 100.0 / speedMultiplier);
        
        // MULES TRAVEL WITH CONVOY - they are removed from town and return after delivery
        // Remove wagons from town (they travel with convoy)
        townStorage[fromTown].wagons -= chosenWagons;
        cargo.wagons = chosenWagons;
        
        // Remove traveling mules from town (they will return after delivery)
        townStorage[fromTown].supplies[MULES] -= totalMules;
        
        // Load supplies (but NOT mules as cargo - they're transport, not cargo)
        double actualLoad = (totalAvailable <= finalCapacity) ? totalAvailable : finalCapacity;
        if (totalAvailable <= finalCapacity) {
            // Take everything except mules
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                if (i != MULES) {  // Mules are transport, not cargo
                    cargo.supplies[i] = townStorage[fromTown].supplies[i];
                    townStorage[fromTown].supplies[i] = 0.0;
                }
            }
        } else {
            // Take proportionally except mules
            double ratio = finalCapacity / totalAvailable;
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                if (i != MULES) {  // Mules are transport, not cargo
                    cargo.supplies[i] = townStorage[fromTown].supplies[i] * ratio;
                    townStorage[fromTown].supplies[i] -= cargo.supplies[i];
                }
            }
        }
        cargo.supplies[MULES] = 0;  // Mules are not cargo (they're transport)
        
        PrintToConvoy("  Loaded cargo: %.1f tons total\n", cargo.getTotal());
        PrintToConvoy("    W:%.1f B:%.1f F:%.1f Wp:%.1f C:%.1f P:%d\n",
                      cargo.supplies[WHEAT], cargo.supplies[BARLEY], 
                      cargo.supplies[ANIMAL_FODDER], cargo.supplies[WEAPONS],
                      cargo.supplies[CLOTHING], (int)cargo.supplies[PEOPLE]);
        PrintToFlow("[TIME %.2f] Convoy #%d departed Town %d: %d mules, %d wagons\n",
                    Time, convoyId, fromTown + 1, totalMules, chosenWagons);
        
        Release(*towns[fromTown]);
        
        // Travel to destination - apply wagon speed penalty if using wagons
        double baseTravelTime = Uniform(townTimes[fromTown] * 0.8, townTimes[fromTown] * 1.2);
        double actualTravelTime = baseTravelTime * weatherDelayMultiplier * speedMultiplier;
        
        const char* weatherStatus = isWeatherActive ? " [BAD WEATHER]" : "";
        PrintToConvoy("  Travel time breakdown:%s\n", weatherStatus);
        PrintToConvoy("    Base: %.2f days | Weather: %.2fx | Transport: %.2fx | Actual: %.2f days\n",
              baseTravelTime, weatherDelayMultiplier, speedMultiplier, actualTravelTime);
        
        Wait(actualTravelTime);
        
        // ===== TRAVEL LOSSES AND CONSUMPTION =====
        PrintToConvoy("  Calculating travel losses...\n");
        
        // People consume food during travel (wheat and barley)
        int peopleCount = (int)cargo.supplies[PEOPLE];
        double foodConsumptionPerPersonPerDay = 0.02; // tons per person per day
        double totalFoodNeeded = peopleCount * foodConsumptionPerPersonPerDay * actualTravelTime;
        
        // Mules consume animal fodder during travel (totalMules already calculated above)
        double fodderConsumptionPerMulePerDay = 0.03; // tons per mule per day (traveling - more than town mules)
        double totalFodderNeeded = totalMules * fodderConsumptionPerMulePerDay * actualTravelTime;
        
        // Consume wheat first, then barley if wheat runs out (for people)
        double wheatConsumed = 0.0;
        double barleyConsumed = 0.0;
        
        if (cargo.supplies[WHEAT] >= totalFoodNeeded) {
            wheatConsumed = totalFoodNeeded;
            cargo.supplies[WHEAT] -= wheatConsumed;
        } else {
            wheatConsumed = cargo.supplies[WHEAT];
            cargo.supplies[WHEAT] = 0.0;
            double remainingFoodNeeded = totalFoodNeeded - wheatConsumed;
            barleyConsumed = (cargo.supplies[BARLEY] >= remainingFoodNeeded) ? 
                             remainingFoodNeeded : cargo.supplies[BARLEY];
            cargo.supplies[BARLEY] -= barleyConsumed;
        }
        
        // Consume animal fodder for mules
        double fodderConsumed = 0.0;
        if (cargo.supplies[ANIMAL_FODDER] >= totalFodderNeeded) {
            fodderConsumed = totalFodderNeeded;
            cargo.supplies[ANIMAL_FODDER] -= fodderConsumed;
        } else {
            fodderConsumed = cargo.supplies[ANIMAL_FODDER];
            cargo.supplies[ANIMAL_FODDER] = 0.0;
        }
        
        // Print consumption summary
        double totalLosses = wheatConsumed + barleyConsumed + fodderConsumed;
        PrintToConvoy("  Travel consumption: %.1f tons total\n", totalLosses);
        PrintToConvoy("    People (%d): %.1f tons wheat + %.1f tons barley\n", 
              peopleCount, wheatConsumed, barleyConsumed);
        PrintToConvoy("    Mules (%d): %.1f tons fodder\n", 
              totalMules, fodderConsumed);
        if (cargo.wagons > 0) {
            PrintToConvoy("    Wagons: %d transported\n", cargo.wagons);
        }
        
        // Arrive at destination
        PrintToMain("[TIME %.2f] Convoy #%d: Arrived at Town %d\n", 
              Time, convoyId, toTown + 1);
        PrintToConvoy("\n[TIME %.2f] Convoy #%d: Arrival at Town %d\n", Time, convoyId, toTown + 1);
        
        // Seize town for unloading
        Seize(*towns[toTown]);
        PrintToConvoy("  Unloading %.1f tons of cargo...\n", cargo.getTotal());
        
        townStorage[toTown].add(cargo);  // This adds supplies AND wagons
        
        // Transfer mules to destination town (they stay here)
        townStorage[toTown].supplies[MULES] += totalMules;
        
        PrintToConvoy("  Town %d now has %.1f tons total\n", toTown + 1, townStorage[toTown].getTotal());
        PrintToConvoy("  Town %d now has %d mules and %d wagons\n", toTown + 1, (int)townStorage[toTown].supplies[MULES], townStorage[toTown].wagons);
        PrintToFlow("[TIME %.2f] Convoy #%d arrived Town %d: +%d mules, +%d wagons, +%.1f tons cargo\n",
                    Time, convoyId, toTown + 1, totalMules, chosenWagons, cargo.getTotal());
        
        Release(*towns[toTown]);
        
        // If this is the last town, record final delivery
        if (toTown == NUM_TOWNS - 1) {
            TotalSuppliesDelivered(cargo.getTotal());
            PrintToMain("              *** Convoy #%d: FINAL DESTINATION REACHED ***\n", convoyId);
            PrintToConvoy("  *** FINAL DESTINATION REACHED ***\n");
        }
        
        // Note: All convoys run independently in parallel
    }
    
public:
    Convoy() : fromTown(0), toTown(1), convoyId(0) {}
    Convoy(int from, int to, int id) : fromTown(from), toTown(to), convoyId(id) {}
};

// ============================================================================
// MAIN
// ============================================================================

int main() {
    // Open all output files (executable runs from output/ directory)
    mainLog = fopen("phase2_main.txt", "w");
    convoyDetails = fopen("phase2_convoy_details.txt", "w");
    townStats = fopen("phase2_town_statistics.txt", "w");
    resourceFlow = fopen("phase2_resource_flow.txt", "w");
    summaryFile = fopen("phase2_summary.txt", "w");
    
    if (!mainLog || !convoyDetails || !townStats || !resourceFlow || !summaryFile) {
        fprintf(stderr, "Error opening output files:\n");
        if (!mainLog) fprintf(stderr, "  - phase2_main.txt\n");
        if (!convoyDetails) fprintf(stderr, "  - phase2_convoy_details.txt\n");
        if (!townStats) fprintf(stderr, "  - phase2_town_statistics.txt\n");
        if (!resourceFlow) fprintf(stderr, "  - phase2_resource_flow.txt\n");
        if (!summaryFile) fprintf(stderr, "  - phase2_summary.txt\n");
        return 1;
    }
    
    SetOutput("phase2_output.txt");  // Keep for SIMLIB statistics
    
    InitializeFacilities();
    InitializeTownSupplies();
    
    // ========== MAIN LOG: Initial Configuration ==========
    PrintToMain("========================================\n");
    PrintToMain("  SUPPLY CHAIN CONVOY SIMULATION\n");
    PrintToMain("  %d Towns in linear chain\n", NUM_TOWNS);
    PrintToMain("  %d convoy routes\n", NUM_TOWNS - 1);
    PrintToMain("========================================\n\n");
    
    PrintToMain("Initial town supplies:\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        PrintToMain("  Town %d: %.1f tons total\n", i + 1, townStorage[i].getTotal());
        fprintf(mainLog, "    ");
        for (int j = 0; j < NUM_SUPPLY_TYPES; j++) {
            if (j == PEOPLE) {
                fprintf(mainLog, "%s: %d people", SupplyNames[j], (int)townStorage[i].supplies[j]);
            } else if (j == MULES) {
                fprintf(mainLog, "%s: %d mules", SupplyNames[j], (int)townStorage[i].supplies[j]);
            } else {
                fprintf(mainLog, "%s: %.1f tons", SupplyNames[j], townStorage[i].supplies[j]);
            }
            if (j < NUM_SUPPLY_TYPES - 1) fprintf(mainLog, ", ");
        }
        fprintf(mainLog, ", Wagons: %d\n", townStorage[i].wagons);
    }
    PrintToMain("\n");
    
    PrintToMain("Convoy capacity limits (tons):\n");
    for (int i = 0; i < NUM_TOWNS - 1; i++) {
        PrintToMain("  Route %d->%d: %.1f tons max\n", i + 1, i + 2, maxConvoyCapacity[i]);
    }
    PrintToMain("\n");
    
    // ========== TOWN STATS: Initial State ==========
    PrintToTown("========================================\n");
    PrintToTown("  INITIAL TOWN SUPPLY STATE\n");
    PrintToTown("========================================\n\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        PrintToTown("Town %d: W:%.0f B:%.0f M:%d F:%.0f Wp:%.0f C:%.0f P:%d Wg:%d\n",
                    i + 1,
                    townStorage[i].supplies[WHEAT],
                    townStorage[i].supplies[BARLEY],
                    (int)townStorage[i].supplies[MULES],
                    townStorage[i].supplies[ANIMAL_FODDER],
                    townStorage[i].supplies[WEAPONS],
                    townStorage[i].supplies[CLOTHING],
                    (int)townStorage[i].supplies[PEOPLE],
                    townStorage[i].wagons);
    }
    PrintToTown("\n");
    
    // ========== CONVOY DETAILS: Header ==========
    PrintToConvoy("========================================\n");
    PrintToConvoy("  CONVOY OPERATIONS LOG\n");
    PrintToConvoy("========================================\n\n");
    
    // ========== RESOURCE FLOW: Header ==========
    PrintToFlow("========================================\n");
    PrintToFlow("  RESOURCE MOVEMENT ANALYSIS\n");
    PrintToFlow("========================================\n\n");
    PrintToFlow("Convoy Routes and Capacities:\n");
    for (int i = 0; i < NUM_TOWNS - 1; i++) {
        PrintToFlow("  Route %d->%d: %.1f tons max capacity\n", i + 1, i + 2, maxConvoyCapacity[i]);
    }
    PrintToFlow("\n");
    
    // Initialize simulation
    Init(0, 100);  // Long enough for all convoys
    
    // Start daily consumption process for all towns
    (new DailyConsumptionProcess)->Activate();
    
    // Start weather system - DISABLED
    // (new BadWeatherEvent)->Activate(Uniform(8, 15));
    
    // Start multiple convoys in parallel (one per route)
    // Stagger convoy starts to avoid initial congestion
    PrintToMain("--- Starting simulation with %d parallel convoys ---\n", NUM_TOWNS - 1);
    PrintToConvoy("Starting %d parallel convoys:\n\n", NUM_TOWNS - 1);
    for (int i = 0; i < NUM_TOWNS - 1; i++) {
        Convoy* convoy = new Convoy(i, i + 1, i + 1);  // Pass convoy ID (1-based)
        convoy->Activate(Time + i * 0.5); // Stagger starts by 0.5 days
        ConvoyCount(1);  // Count this convoy
    }
    
    Run();
    
    // ========== MAIN LOG: Final Results ==========
    PrintToMain("\n========================================\n");
    PrintToMain("  SIMULATION COMPLETED\n");
    PrintToMain("========================================\n\n");
    
    // ========== TOWN STATS: Final State ==========
    PrintToTown("\n========================================\n");
    PrintToTown("  FINAL TOWN SUPPLY STATE\n");
    PrintToTown("========================================\n\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        PrintToTown("Town %d: W:%.0f B:%.0f M:%d F:%.0f Wp:%.0f C:%.0f P:%d Wg:%d (%.1f tons total)\n",
                    i + 1,
                    townStorage[i].supplies[WHEAT],
                    townStorage[i].supplies[BARLEY],
                    (int)townStorage[i].supplies[MULES],
                    townStorage[i].supplies[ANIMAL_FODDER],
                    townStorage[i].supplies[WEAPONS],
                    townStorage[i].supplies[CLOTHING],
                    (int)townStorage[i].supplies[PEOPLE],
                    townStorage[i].wagons,
                    townStorage[i].getTotal());
    }
    
    // ========== SUMMARY FILE: Executive Summary ==========
    PrintToSummary("========================================\n");
    PrintToSummary("  SIMULATION EXECUTIVE SUMMARY\n");
    PrintToSummary("========================================\n\n");
    
    PrintToSummary("Simulation Parameters:\n");
    PrintToSummary("  - Number of towns: %d\n", NUM_TOWNS);
    PrintToSummary("  - Number of convoy routes: %d\n", NUM_TOWNS - 1);
    PrintToSummary("  - Simulation duration: %.0f days\n", Time);
    PrintToSummary("  - Wagon capacity: %.1f tons\n", WAGON_CAPACITY);
    PrintToSummary("  - Mule carry capacity: %.2f tons\n", MULE_CARRY_CAPACITY);
    PrintToSummary("  - Wagon speed penalty: %.0f%%\n", (WAGON_SPEED_PENALTY - 1.0) * 100);
    PrintToSummary("\n");
    
    PrintToSummary("Key Results:\n");
    PrintToSummary("  - Total convoys: %d\n", (int)ConvoyCount.Number());
    PrintToSummary("  - Supplies delivered to Town %d: %.2f tons\n", NUM_TOWNS, TotalSuppliesDelivered.Sum());
    PrintToSummary("\n");
    
    PrintToSummary("Final Town States:\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        PrintToSummary("  Town %d: %.1f tons, %d mules, %d wagons, %d people\n",
                      i + 1,
                      townStorage[i].getTotal(),
                      (int)townStorage[i].supplies[MULES],
                      townStorage[i].wagons,
                      (int)townStorage[i].supplies[PEOPLE]);
    }
    PrintToSummary("\n");
    
    // Keep SIMLIB statistics in main output file
    Print("\n========================================\n");
    Print("  SIMULATION STATISTICS\n");
    Print("========================================\n\n");
    
    Print("Final town supplies:\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        Print("  Town %d: %.1f tons\n", i + 1, townStorage[i].getTotal());
        townStorage[i].print("    ");
    }
    Print("\n");
    
    ConvoyCount.Output();
    TotalSuppliesDelivered.Output();
    
    SIMLIB_statistics.Output();
    
    // Close all output files
    fclose(mainLog);
    fclose(convoyDetails);
    fclose(townStats);
    fclose(resourceFlow);
    fclose(summaryFile);
    
    return 0;
}


