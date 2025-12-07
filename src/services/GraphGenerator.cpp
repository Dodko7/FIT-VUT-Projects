#include "GraphGenerator.h"
#include "SimulationState.h"
#include "../config/Config.h"
#include "../models/ExperimentResult.h"
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

void GraphGenerator::GenerateCostGraph(int currentDay) {
    auto* state = SimulationState::GetInstance();
    
    const int GRAPH_HEIGHT = Config::COST_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = Config::COST_GRAPH_WIDTH;
    
    // Dynamically scale based on actual maximum cost with 10% padding
    double maxCostInHistory = 0.0;
    for (double cost : state->campaignCostHistory) {
        if (cost > maxCostInHistory) maxCostInHistory = cost;
    }
    const double MAX_COST = maxCostInHistory * 1.1;
    
    state->costGraphFile.open("output/campaign_cost_graph.txt");
    
    state->costGraphFile << "\n";
    state->costGraphFile << "========================================\n";
    state->costGraphFile << "  CAMPAIGN COST GRAPH\n";
    state->costGraphFile << "  Day " << currentDay << " of " << Config::runtime.simulationDays << "\n";
    state->costGraphFile << "========================================\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 10, ' '));
    
    // Plot data points
    for (int day = 0; day < (int)state->campaignCostHistory.size() && day < GRAPH_WIDTH; day++) {
        double cost = state->campaignCostHistory[day];
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
        state->costGraphFile << std::to_string((int)cost);
        
        int numDigits = std::to_string((int)cost).length();
        for (int i = numDigits; i < 7; i++) {
            state->costGraphFile << " ";
        }
        
        state->costGraphFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            state->costGraphFile << graph[y][x];
        }
        state->costGraphFile << "\n";
    }
    
    // X-axis
    state->costGraphFile << "       +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        state->costGraphFile << "-";
    }
    state->costGraphFile << "\n";
    
    // X-axis labels
    state->costGraphFile << "        0";
    for (int i = 10; i <= 100; i += 10) {
        if (i <= GRAPH_WIDTH) {
            int spaces = 10 - std::to_string(i).length();
            for (int j = 0; j < spaces; j++) state->costGraphFile << " ";
            state->costGraphFile << i;
        }
    }
    state->costGraphFile << "\n";
    state->costGraphFile << "\n        Days ->\n";
    state->costGraphFile << "\nCost (money) ^\n\n";
    
    // Print data table
    state->costGraphFile << "Day-by-day costs:\n";
    for (int i = 0; i < (int)state->campaignCostHistory.size(); i++) {
        state->costGraphFile << "Day " << i << ": " << state->campaignCostHistory[i] << " money\n";
    }
    
    state->costGraphFile.close();
}

void GraphGenerator::GenerateSupplyGraph(int currentDay) {
    auto* state = SimulationState::GetInstance();
    
    const int GRAPH_HEIGHT = Config::SUPPLY_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = Config::SUPPLY_GRAPH_WIDTH;
    const double MAX_SUPPLY = Config::SUPPLY_GRAPH_MAX;
    
    state->graphFile.open("output/army_supply_graph.txt");
    
    state->graphFile << "\n";
    state->graphFile << "========================================\n";
    state->graphFile << "  ARMY SUPPLY GRAPH\n";
    state->graphFile << "  Day " << currentDay << " of " << Config::runtime.simulationDays << "\n";
    state->graphFile << "========================================\n\n";
    
    // Create graph grid
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 10, ' '));
    
    // Plot data points
    for (int day = 0; day < (int)state->town5SuppliesHistory.size() && day < GRAPH_WIDTH; day++) {
        double supply = state->town5SuppliesHistory[day];
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
        state->graphFile << std::to_string((int)supply);
        
        int numDigits = std::to_string((int)supply).length();
        for (int i = numDigits; i < 4; i++) {
            state->graphFile << " ";
        }
        
        state->graphFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            state->graphFile << graph[y][x];
        }
        state->graphFile << "\n";
    }
    
    // X-axis
    state->graphFile << "    +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        state->graphFile << "-";
    }
    state->graphFile << "\n";
    
    // X-axis labels
    state->graphFile << "     0";
    for (int i = 10; i <= 100; i += 10) {
        if (i <= GRAPH_WIDTH) {
            int spaces = 10 - std::to_string(i).length();
            for (int j = 0; j < spaces; j++) state->graphFile << " ";
            state->graphFile << i;
        }
    }
    state->graphFile << "\n";
    state->graphFile << "\n     Days ->\n";
    state->graphFile << "\nSupplies (tons) ^\n\n";
    
    // Print data table
    state->graphFile << "Day-by-day data:\n";
    for (int i = 0; i < (int)state->town5SuppliesHistory.size(); i++) {
        state->graphFile << "Day " << i << ": " << state->town5SuppliesHistory[i] << " tons\n";
    }
    
    state->graphFile.close();
}

void GraphGenerator::GenerateDailyCostGraph(int dayNumber) {
    auto* state = SimulationState::GetInstance();
    
    // Only generate the final graph at the end of simulation
    if (dayNumber != Config::runtime.simulationDays - 1) {
        return;
    }
    
    const int GRAPH_HEIGHT = Config::DAILY_COST_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = Config::DAILY_COST_GRAPH_WIDTH;
    
    // Find max daily cost for scaling
    double maxDailyCost = 0.0;
    for (double cost : state->dailyCosts) {
        if (cost > maxDailyCost) maxDailyCost = cost;
    }
    if (maxDailyCost == 0.0) maxDailyCost = 1.0;
    
    std::ofstream dailyCostFile("output/daily_cost_graph.txt");
    
    dailyCostFile << "\n";
    dailyCostFile << "========================================\n";
    dailyCostFile << "  DAILY CAMPAIGN COST\n";
    dailyCostFile << "  (Money spent each day)\n";
    dailyCostFile << "========================================\n\n";
    
    dailyCostFile << "Total days: " << state->dailyCosts.size() << "\n";
    dailyCostFile << "Max daily cost: " << (int)maxDailyCost << " money\n";
    dailyCostFile << "Total campaign cost: " << (int)state->campaignCost << " money\n\n";
    
    // Create graph
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 5, ' '));
    
    int totalDays = state->dailyCosts.size();
    
    for (int day = 0; day < totalDays; day++) {
        int x = (day * GRAPH_WIDTH) / totalDays;
        if (x >= GRAPH_WIDTH) x = GRAPH_WIDTH - 1;
        
        double cost = state->dailyCosts[day];
        int y = GRAPH_HEIGHT - (int)((cost / maxDailyCost) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        graph[y][x] = '.';
    }
    
    // Print graph
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double cost = maxDailyCost - (y * maxDailyCost / GRAPH_HEIGHT);
        std::string costStr = std::to_string((int)cost);
        dailyCostFile << costStr;
        
        for (int i = costStr.length(); i < 7; i++) {
            dailyCostFile << " ";
        }
        
        dailyCostFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            dailyCostFile << graph[y][x];
        }
        dailyCostFile << "\n";
    }
    
    dailyCostFile << "       +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        dailyCostFile << "-";
    }
    dailyCostFile << "\n";
    
    dailyCostFile << "        Day 0 to " << (totalDays - 1) << "\n\n";
    
    // Print daily cost list
    dailyCostFile << "Daily costs breakdown:\n";
    for (int day = 0; day < (int)state->dailyCosts.size(); day++) {
        dailyCostFile << "  Day " << day << ": " << (int)state->dailyCosts[day] << " money\n";
    }
    
    dailyCostFile.close();
}

void GraphGenerator::GenerateAllTownsWeeklyCostGraph(int dayNumber) {
    auto* state = SimulationState::GetInstance();
    
    // Only generate the final graph at the end of simulation
    if (dayNumber != Config::runtime.simulationDays - 1) {
        return;
    }
    
    const int GRAPH_HEIGHT = Config::ALL_TOWNS_WEEKLY_COST_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = Config::ALL_TOWNS_WEEKLY_COST_GRAPH_WIDTH;
    
    std::ofstream allTownsFile("output/weekly_cost_graph.txt");
    
    allTownsFile << "\n";
    allTownsFile << "========================================\n";
    allTownsFile << "  ALL TOWNS WEEKLY COST (COMBINED)\n";
    allTownsFile << "  (Total money spent on all routes per week)\n";
    allTownsFile << "========================================\n\n";
    
    // Find max number of weeks across all routes
    int maxWeeks = 0;
    for (int route = 0; route < 4; route++) {
        if ((int)state->weeklyCostsAllRoutes[route].size() > maxWeeks) {
            maxWeeks = state->weeklyCostsAllRoutes[route].size();
        }
    }
    
    int displayWeeks = std::min(maxWeeks, GRAPH_WIDTH);
    
    // Calculate combined weekly costs across all routes
    std::vector<double> combinedWeeklyCosts(maxWeeks, 0.0);
    for (int week = 0; week < maxWeeks; week++) {
        for (int route = 0; route < 4; route++) {
            if (week < (int)state->weeklyCostsAllRoutes[route].size()) {
                combinedWeeklyCosts[week] += state->weeklyCostsAllRoutes[route][week];
            }
        }
    }
    
    // Find max combined weekly cost for scaling
    double maxWeeklyCost = 0.0;
    for (double cost : combinedWeeklyCosts) {
        if (cost > maxWeeklyCost) maxWeeklyCost = cost;
    }
    if (maxWeeklyCost == 0.0) maxWeeklyCost = 1.0;
    
    // Calculate total cost and route-specific totals
    double totalCost = 0.0;
    for (double cost : combinedWeeklyCosts) {
        totalCost += cost;
    }
    
    allTownsFile << "Total weeks tracked: " << maxWeeks << "\n";
    allTownsFile << "Max weekly cost (combined): " << (int)maxWeeklyCost << " money\n";
    allTownsFile << "Total campaign cost: " << (int)totalCost << " money\n\n";
    
    // Print route breakdown for reference
    allTownsFile << "Route contribution to total:\n";
    const char* routeNames[] = {"Town 1->2", "Town 2->3", "Town 3->4", "Town 4->5"};
    for (int route = 0; route < 4; route++) {
        double totalRouteCost = 0.0;
        for (double cost : state->weeklyCostsAllRoutes[route]) {
            totalRouteCost += cost;
        }
        allTownsFile << "  " << routeNames[route] << ": " << (int)totalRouteCost << " money\n";
    }
    allTownsFile << "\n";
    
    // Create graph
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(displayWeeks + 5, ' '));
    
    // Plot combined costs
    for (int week = 0; week < displayWeeks; week++) {
        double cost = combinedWeeklyCosts[week];
        if (cost > 0) {
            int y = GRAPH_HEIGHT - (int)((cost / maxWeeklyCost) * GRAPH_HEIGHT);
            if (y < 0) y = 0;
            if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
            
            graph[y][week] = '.';
        }
    }
    
    // Print graph
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double cost = maxWeeklyCost - (y * maxWeeklyCost / GRAPH_HEIGHT);
        std::string costStr = std::to_string((int)cost);
        allTownsFile << costStr;
        
        for (int i = costStr.length(); i < 7; i++) {
            allTownsFile << " ";
        }
        
        allTownsFile << "| ";
        
        for (int x = 0; x < displayWeeks; x++) {
            allTownsFile << graph[y][x];
        }
        allTownsFile << "\n";
    }
    
    allTownsFile << "       +";
    for (int i = 0; i < displayWeeks; i++) {
        allTownsFile << "-";
    }
    allTownsFile << "\n";
    
    allTownsFile << "        Week 0 to " << (displayWeeks - 1) << "\n\n";
    
    // Print combined weekly costs breakdown
    allTownsFile << "Combined weekly costs:\n";
    for (int week = 0; week < maxWeeks; week++) {
        allTownsFile << "  Week " << week << ": " << (int)combinedWeeklyCosts[week] << " money\n";
    }
    
    allTownsFile.close();
}

void GraphGenerator::GenerateExperimentGraph(const std::string& experimentType) {
    auto* state = SimulationState::GetInstance();
    
    const int GRAPH_HEIGHT = 30;
    const int GRAPH_WIDTH = Config::EXPERIMENT_RUNS;
    
    if (state->experimentResults.empty()) return;
    
    double maxCost = 0.0;
    for (const auto& result : state->experimentResults) {
        if (result.totalCost > maxCost) maxCost = result.totalCost;
    }
    maxCost *= 1.1;
    
    std::ofstream expFile("output/experiment_results.txt");
    
    expFile << "\n";
    expFile << "========================================\n";
    expFile << "  EXPERIMENT RESULTS\n";
    expFile << "  Type: " << experimentType << "\n";
    expFile << "  Runs: " << Config::EXPERIMENT_RUNS << "\n";
    expFile << "========================================\n\n";
    
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH * 3 + 10, ' '));
    
    for (int run = 0; run < (int)state->experimentResults.size(); run++) {
        double cost = state->experimentResults[run].totalCost;
        int y = GRAPH_HEIGHT - (int)((cost / maxCost) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        int x = run * 3;
        graph[y][x] = '.';
    }
    
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
    
    expFile << "        +";
    for (int i = 0; i < GRAPH_WIDTH * 3; i++) {
        expFile << "-";
    }
    expFile << "\n";
    
    expFile << "         ";
    for (int run = 0; run < Config::EXPERIMENT_RUNS; run++) {
        expFile << (run + 1);
        if (run < Config::EXPERIMENT_RUNS - 1) expFile << "  ";
    }
    expFile << "\n";
    expFile << "\n         Run Number ->\n";
    expFile << "\nCost (money) ^\n\n";
    
    expFile << "Detailed Results:\n";
    expFile << "Run | Parameter Value | Total Cost | Final Supplies | Convoys\n";
    expFile << "----+----------------+------------+----------------+--------\n";
    for (int run = 0; run < (int)state->experimentResults.size(); run++) {
        expFile << std::setw(3) << (run + 1) << " | ";
        expFile << std::setw(14) << std::fixed << std::setprecision(2) << state->experimentResults[run].parameterValue << " | ";
        expFile << std::setw(10) << (int)state->experimentResults[run].totalCost << " | ";
        expFile << std::setw(14) << std::fixed << std::setprecision(1) << state->experimentResults[run].finalSupplies << " | ";
        expFile << std::setw(6) << state->experimentResults[run].convoyCount << "\n";
    }
    
    expFile << "\n";
    expFile << "Summary Statistics:\n";
    double avgCost = 0.0;
    for (const auto& result : state->experimentResults) {
        avgCost += result.totalCost;
    }
    avgCost /= state->experimentResults.size();
    expFile << "Average cost across all runs: " << (int)avgCost << " money\n";
    expFile << "Minimum cost: " << (int)state->experimentResults[0].totalCost << " money (Run 1)\n";
    expFile << "Maximum cost: " << (int)state->experimentResults[state->experimentResults.size()-1].totalCost << " money (Run " << state->experimentResults.size() << ")\n";
    
    expFile.close();
}

void GraphGenerator::GenerateTown5DeliveriesGraph(int dayNumber) {
    auto* state = SimulationState::GetInstance();
    
    // Only generate the final graph at the end of simulation
    if (dayNumber != Config::runtime.simulationDays - 1) {
        return;
    }
    
    const int GRAPH_HEIGHT = Config::DELIVERIES_GRAPH_HEIGHT;
    const int GRAPH_WIDTH = Config::DELIVERIES_GRAPH_WIDTH;
    
    // Find max cumulative delivery for scaling
    double maxTotalDelivered = 0.0;
    for (double total : state->town5TotalDeliveredHistory) {
        if (total > maxTotalDelivered) maxTotalDelivered = total;
    }
    if (maxTotalDelivered == 0.0) maxTotalDelivered = 1.0;
    
    std::ofstream deliveriesFile("output/army_deliveries_graph.txt");
    
    deliveriesFile << "\n";
    deliveriesFile << "========================================\n";
    deliveriesFile << "  ARMY TOTAL DELIVERIES (CUMULATIVE)\n";
    deliveriesFile << "  (Total supplies delivered over time)\n";
    deliveriesFile << "========================================\n\n";
    
    deliveriesFile << "Total days: " << state->town5TotalDeliveredHistory.size() << "\n";
    deliveriesFile << "Final total delivered: " << (int)maxTotalDelivered << " tons\n\n";
    
    // Create graph
    std::vector<std::string> graph(GRAPH_HEIGHT + 1, std::string(GRAPH_WIDTH + 5, ' '));
    
    int totalDays = state->town5TotalDeliveredHistory.size();
    
    for (int day = 0; day < totalDays; day++) {
        int x = (day * GRAPH_WIDTH) / totalDays;
        if (x >= GRAPH_WIDTH) x = GRAPH_WIDTH - 1;
        
        double totalDelivered = state->town5TotalDeliveredHistory[day];
        int y = GRAPH_HEIGHT - (int)((totalDelivered / maxTotalDelivered) * GRAPH_HEIGHT);
        if (y < 0) y = 0;
        if (y > GRAPH_HEIGHT) y = GRAPH_HEIGHT;
        
        graph[y][x] = '.';
    }
    
    // Print graph
    for (int y = 0; y <= GRAPH_HEIGHT; y++) {
        double totalDelivered = maxTotalDelivered - (y * maxTotalDelivered / GRAPH_HEIGHT);
        std::string deliveryStr = std::to_string((int)totalDelivered);
        deliveriesFile << deliveryStr;
        
        for (int i = deliveryStr.length(); i < 7; i++) {
            deliveriesFile << " ";
        }
        
        deliveriesFile << "| ";
        
        for (int x = 0; x < GRAPH_WIDTH; x++) {
            deliveriesFile << graph[y][x];
        }
        deliveriesFile << "\n";
    }
    
    deliveriesFile << "       +";
    for (int i = 0; i < GRAPH_WIDTH; i++) {
        deliveriesFile << "-";
    }
    deliveriesFile << "\n";
    
    deliveriesFile << "        Day 0 to " << (totalDays - 1) << "\n\n";
    
    // Print cumulative deliveries list
    deliveriesFile << "Cumulative deliveries by day:\n";
    for (int day = 0; day < (int)state->town5TotalDeliveredHistory.size(); day++) {
        deliveriesFile << "  Day " << day << ": " << std::fixed << std::setprecision(1) 
                       << state->town5TotalDeliveredHistory[day] << " tons total delivered\n";
    }
    
    deliveriesFile.close();
}
