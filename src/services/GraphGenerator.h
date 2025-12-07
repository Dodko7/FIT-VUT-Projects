#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <string>

class GraphGenerator {
public:
    static void GenerateSupplyGraph(int currentDay);
    static void GenerateCostGraph(int currentDay);
    static void GenerateDailyCostGraph(int dayNumber);
    static void GenerateTown1WeeklyCostGraph(int dayNumber);
    static void GenerateAllTownsWeeklyCostGraph(int dayNumber);
    static void GenerateExperimentGraph(const std::string& experimentType);
};

#endif // GRAPH_GENERATOR_H
