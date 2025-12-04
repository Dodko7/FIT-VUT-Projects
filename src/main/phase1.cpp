#include <simlib.h>
#include <vector>
#include <string>
#include <map>

// ============================================================================
// SUPPLY TYPES
// ============================================================================

enum SupplyType {
    WHEAT,
    BARLEY,
    LIVESTOCK,
    ANIMAL_FODDER,
    WEAPONS,
    CLOTHING,
    PEOPLE,
    NUM_SUPPLY_TYPES
};

const char* SupplyNames[] = {
    "Wheat",
    "Barley",
    "Livestock",
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

// ============================================================================
// TOWN SUPPLY STORAGE
// ============================================================================

struct TownSupplies {
    double supplies[NUM_SUPPLY_TYPES];
    
    TownSupplies() {
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            supplies[i] = 0.0;
        }
    }
    
    void add(const TownSupplies& other) {
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            supplies[i] += other.supplies[i];
        }
    }
    
    double getTotal() const {
        double total = 0.0;
        for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
            // Don't count people and livestock in tonnage
            if (i != PEOPLE && i != LIVESTOCK) {
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
            } else if (i == LIVESTOCK) {
                Print("%s: %d animals", SupplyNames[i], (int)supplies[i]);
            } else {
                Print("%s: %.1f tons", SupplyNames[i], supplies[i]);
            }
            if (i < NUM_SUPPLY_TYPES - 1) Print(", ");
        }
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
    townStorage[0].supplies[LIVESTOCK] = 50.0;  // 50 animals
    townStorage[0].supplies[ANIMAL_FODDER] = 120.0;
    townStorage[0].supplies[WEAPONS] = 30.0;
    townStorage[0].supplies[CLOTHING] = 60.0;
    townStorage[0].supplies[PEOPLE] = 20.0;  // 20 people
    
    // Town 2
    townStorage[1].supplies[WHEAT] = 50.0;
    townStorage[1].supplies[BARLEY] = 40.0;
    townStorage[1].supplies[LIVESTOCK] = 20.0;  // 20 animals
    townStorage[1].supplies[ANIMAL_FODDER] = 60.0;
    townStorage[1].supplies[WEAPONS] = 15.0;
    townStorage[1].supplies[CLOTHING] = 30.0;
    townStorage[1].supplies[PEOPLE] = 10.0;  // 10 people
    
    // Town 3
    townStorage[2].supplies[WHEAT] = 70.0;
    townStorage[2].supplies[BARLEY] = 60.0;
    townStorage[2].supplies[LIVESTOCK] = 30.0;  // 30 animals
    townStorage[2].supplies[ANIMAL_FODDER] = 80.0;
    townStorage[2].supplies[WEAPONS] = 20.0;
    townStorage[2].supplies[CLOTHING] = 40.0;
    townStorage[2].supplies[PEOPLE] = 15.0;  // 15 people
    
    // Town 4
    townStorage[3].supplies[WHEAT] = 40.0;
    townStorage[3].supplies[BARLEY] = 30.0;
    townStorage[3].supplies[LIVESTOCK] = 15.0;  // 15 animals
    townStorage[3].supplies[ANIMAL_FODDER] = 50.0;
    townStorage[3].supplies[WEAPONS] = 10.0;
    townStorage[3].supplies[CLOTHING] = 25.0;
    townStorage[3].supplies[PEOPLE] = 8.0;  // 8 people
    
    // Town 5
    townStorage[4].supplies[WHEAT] = 60.0;
    townStorage[4].supplies[BARLEY] = 50.0;
    townStorage[4].supplies[LIVESTOCK] = 25.0;  // 25 animals
    townStorage[4].supplies[ANIMAL_FODDER] = 70.0;
    townStorage[4].supplies[WEAPONS] = 18.0;
    townStorage[4].supplies[CLOTHING] = 35.0;
    townStorage[4].supplies[PEOPLE] = 12.0;  // 12 people
    
    // Town 6
    townStorage[5].supplies[WHEAT] = 30.0;
    townStorage[5].supplies[BARLEY] = 25.0;
    townStorage[5].supplies[LIVESTOCK] = 10.0;  // 10 animals
    townStorage[5].supplies[ANIMAL_FODDER] = 40.0;
    townStorage[5].supplies[WEAPONS] = 8.0;
    townStorage[5].supplies[CLOTHING] = 20.0;
    townStorage[5].supplies[PEOPLE] = 6.0;  // 6 people
    
    // Town 7
    townStorage[6].supplies[WHEAT] = 45.0;
    townStorage[6].supplies[BARLEY] = 35.0;
    townStorage[6].supplies[LIVESTOCK] = 18.0;  // 18 animals
    townStorage[6].supplies[ANIMAL_FODDER] = 55.0;
    townStorage[6].supplies[WEAPONS] = 12.0;
    townStorage[6].supplies[CLOTHING] = 28.0;
    townStorage[6].supplies[PEOPLE] = 9.0;  // 9 people
    
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

class DailyConsumptionEvent : public Event {
    void Behavior();
};

void DailyConsumptionEvent::Behavior() {
    // Each town consumes supplies daily
    int totalDeaths = 0;
    
    for (int i = 0; i < NUM_TOWNS; i++) {
        int townPeople = (int)townStorage[i].supplies[PEOPLE];
        int townLivestock = (int)townStorage[i].supplies[LIVESTOCK];
        
        // People consume food (wheat/barley)
        double peopleFood = townPeople * 0.02; // tons per person per day
        if (townStorage[i].supplies[WHEAT] >= peopleFood) {
            townStorage[i].supplies[WHEAT] -= peopleFood;
        } else {
            double remaining = peopleFood - townStorage[i].supplies[WHEAT];
            townStorage[i].supplies[WHEAT] = 0.0;
            if (townStorage[i].supplies[BARLEY] >= remaining) {
                townStorage[i].supplies[BARLEY] -= remaining;
            } else {
                townStorage[i].supplies[BARLEY] = 0.0;
            }
        }
        
        // Livestock consume fodder
        double livestockFodder = townLivestock * 0.05; // tons per animal per day
        if (townStorage[i].supplies[ANIMAL_FODDER] >= livestockFodder) {
            townStorage[i].supplies[ANIMAL_FODDER] -= livestockFodder;
        } else {
            // Not enough fodder - animals may die
            townStorage[i].supplies[ANIMAL_FODDER] = 0.0;
            double lossRate = 0.05; // 5% die per day without food
            int deaths = (int)(townLivestock * lossRate);
            if (deaths > townLivestock) deaths = townLivestock;
            townStorage[i].supplies[LIVESTOCK] -= deaths;
            totalDeaths += deaths;
        }
    }
    
    // Print summary every 5 days
    int currentDay = (int)Time;
    if (currentDay % 5 == 0 && currentDay > 0) {
        Print("[DAY %d] Daily consumption report: ", currentDay);
        if (totalDeaths > 0) {
            Print("%d animals died from starvation across all towns\n", totalDeaths);
        } else {
            Print("All towns consuming supplies normally\n");
        }
    }
    
    // Schedule next daily consumption
    (new DailyConsumptionEvent)->Activate(Time + 1.0);
}

// ============================================================================
// CONVOY PROCESS - Transports supplies from one town to next
// ============================================================================

class Convoy : public Process {
    int fromTown;  // Index of origin town
    int toTown;    // Index of destination town
    TownSupplies cargo;
    
    void Behavior() {
        Print("\n[TIME %.2f] Convoy #%d: Starting from Town %d to Town %d\n", 
              Time, (int)ConvoyCount.Number() + 1, fromTown + 1, toTown + 1);
        
        ConvoyCount(1);  // Count this convoy
        
        // Load supplies from origin town
        Seize(*towns[fromTown]);
        
        Print("              Loading supplies from Town %d...\n", fromTown + 1);
        
        // Calculate how much to load (limited by max capacity)
        double maxCapacity = maxConvoyCapacity[fromTown];
        double totalAvailable = townStorage[fromTown].getTotal();
        
        if (totalAvailable <= maxCapacity) {
            // Take all supplies
            cargo = townStorage[fromTown];
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                townStorage[fromTown].supplies[i] = 0.0;
            }
        } else {
            // Take proportionally to fit capacity
            double ratio = maxCapacity / totalAvailable;
            for (int i = 0; i < NUM_SUPPLY_TYPES; i++) {
                cargo.supplies[i] = townStorage[fromTown].supplies[i] * ratio;
                townStorage[fromTown].supplies[i] -= cargo.supplies[i];
            }
        }
        
        Print("              Loaded: %.1f tons total\n", cargo.getTotal());
        cargo.print("              ");
        
        Release(*towns[fromTown]);
        
        // Travel to destination
        double baseTravelTime = Uniform(townTimes[fromTown] * 0.8, townTimes[fromTown] * 1.2);
        double actualTravelTime = baseTravelTime * weatherDelayMultiplier;
        
        const char* weatherStatus = isWeatherActive ? " [BAD WEATHER]" : "";
        Print("              Traveling...%s Base: %.2f days | Weather: %.2fx | Actual: %.2f days\n",
              weatherStatus, baseTravelTime, weatherDelayMultiplier, actualTravelTime);
        
        Wait(actualTravelTime);
        
        // ===== TRAVEL LOSSES AND CONSUMPTION =====
        Print("              Calculating travel losses...\n");
        
        // People consume food during travel (wheat and barley)
        int peopleCount = (int)cargo.supplies[PEOPLE];
        double foodConsumptionPerPersonPerDay = 0.02; // tons per person per day
        double totalFoodNeeded = peopleCount * foodConsumptionPerPersonPerDay * actualTravelTime;
        
        // Consume wheat first, then barley if wheat runs out
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
        
        // Livestock consumes animal fodder during travel
        int livestockCount = (int)cargo.supplies[LIVESTOCK];
        double fodderConsumptionPerAnimalPerDay = 0.05; // tons per animal per day
        double totalFodderNeeded = livestockCount * fodderConsumptionPerAnimalPerDay * actualTravelTime;
        
        double fodderConsumed = (cargo.supplies[ANIMAL_FODDER] >= totalFodderNeeded) ?
                                totalFodderNeeded : cargo.supplies[ANIMAL_FODDER];
        cargo.supplies[ANIMAL_FODDER] -= fodderConsumed;
        
        // Livestock losses due to travel distance/time (animals die on the road)
        // Loss rate increases with travel time: base 2% + 0.5% per day
        double livestockLossRate = 0.02 + (0.005 * actualTravelTime);
        int livestockDeaths = (int)(livestockCount * livestockLossRate);
        
        // If not enough fodder, additional livestock losses occur
        double fodderShortage = totalFodderNeeded - fodderConsumed;
        if (fodderShortage > 0.0) {
            double additionalLossRate = 0.1 * (fodderShortage / totalFodderNeeded);
            livestockDeaths += (int)(livestockCount * additionalLossRate);
        }
        
        // Make sure we don't kill more animals than we have
        if (livestockDeaths > livestockCount) livestockDeaths = livestockCount;
        
        cargo.supplies[LIVESTOCK] -= livestockDeaths;
        if (cargo.supplies[LIVESTOCK] < 0.0) cargo.supplies[LIVESTOCK] = 0.0;
        
        // Print consumption and losses summary
        double totalLosses = wheatConsumed + barleyConsumed + fodderConsumed;
        Print("              Travel losses: %.1f tons of supplies, %d animals died\n", 
              totalLosses, livestockDeaths);
        Print("                - %d people consumed: %.1f tons wheat, %.1f tons barley\n", 
              peopleCount, wheatConsumed, barleyConsumed);
        Print("                - %d livestock consumed: %.1f tons fodder\n", 
              livestockCount, fodderConsumed);
        Print("                - %d animals died (%.1f%% loss rate)\n", 
              livestockDeaths, livestockLossRate * 100.0);
        
        // Arrive at destination
        Print("[TIME %.2f] Convoy #%d: Arrived at Town %d\n", 
              Time, (int)ConvoyCount.Number(), toTown + 1);
        
        // Seize town for unloading
        Seize(*towns[toTown]);
        Print("              Unloading %.1f tons...\n", cargo.getTotal());
        
        townStorage[toTown].add(cargo);
        
        Print("              Town %d now has %.1f tons total\n", toTown + 1, townStorage[toTown].getTotal());
        
        Release(*towns[toTown]);
        
        // If this is the last town, record final delivery
        if (toTown == NUM_TOWNS - 1) {
            TotalSuppliesDelivered(cargo.getTotal());
            Print("              *** FINAL DESTINATION REACHED ***\n");
        } else {
            // Generate next convoy from this town to next town
            Convoy* nextConvoy = new Convoy();
            nextConvoy->fromTown = toTown;
            nextConvoy->toTown = toTown + 1;
            nextConvoy->Activate();
        }
    }
    
public:
    Convoy() : fromTown(0), toTown(1) {}
};

// ============================================================================
// MAIN
// ============================================================================

int main() {
    SetOutput("phase1_output.txt");
    
    InitializeFacilities();
    InitializeTownSupplies();
    
    Print("========================================\n");
    Print("  SUPPLY CHAIN CONVOY SIMULATION\n");
    Print("  %d Towns in linear chain\n", NUM_TOWNS);
    Print("  %d convoy routes\n", NUM_TOWNS - 1);
    Print("========================================\n\n");
    
    Print("Initial town supplies:\n");
    for (int i = 0; i < NUM_TOWNS; i++) {
        Print("  Town %d: %.1f tons total\n", i + 1, townStorage[i].getTotal());
        townStorage[i].print("    ");
    }
    Print("\n");
    
    Print("Convoy capacity limits (tons):\n");
    for (int i = 0; i < NUM_TOWNS - 1; i++) {
        Print("  Route %d->%d: %.1f tons max\n", i + 1, i + 2, maxConvoyCapacity[i]);
    }
    Print("\n");
    
    // Initialize simulation
    Init(0, 1000);  // Long enough for all convoys
    
    // Start daily consumption for all towns
    (new DailyConsumptionEvent)->Activate(1.0);
    
    // Start weather system - DISABLED
    // (new BadWeatherEvent)->Activate(Uniform(8, 15));
    
    // Start first convoy from Town 1 to Town 2
    Convoy* firstConvoy = new Convoy();
    firstConvoy->Activate();
    
    Print("--- Starting simulation ---\n");
    Run();
    
    // Output final results
    Print("\n========================================\n");
    Print("  SIMULATION RESULTS\n");
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
    
    return 0;
}


