#ifndef EXPERIMENT_RESULT_H
#define EXPERIMENT_RESULT_H

struct ExperimentResult {
    double parameterValue;      // The varied parameter value
    double totalCost;           // Total campaign cost
    double finalSupplies;       // Final Town 5 supplies
    int convoyCount;            // Total convoys
};

#endif // EXPERIMENT_RESULT_H
