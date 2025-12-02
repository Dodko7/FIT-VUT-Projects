#include <simlib.h>

// 1. Define global objects (facilities, queues, statistics)
Facility Server("Server");
Histogram TimeInSystem("Time in system", 0, 25, 20);

// 2. Define Process/Event classes with Behavior() methods
class Customer : public Process {
    double ArrivalTime;    // Track when customer arrived
    void Behavior() {
        ArrivalTime = Time;        // Record arrival time
        Seize(Server);             // Get the server
        Wait(10);                  // Use it for 10 time units
        Release(Server);           // Release it
        TimeInSystem(Time - ArrivalTime);  // Record total time spent
    }
};

class Generator : public Event {
    void Behavior() {
        (new Customer)->Activate();  // Create new customer
        Activate(Time + Exponential(5)); // Schedule next arrival
    }
};

// 3. Main experiment
int main() {
    SetOutput("output.txt");       // Set output file
    Init(0, 1000);                 // Initialize: start=0, end=1000
    (new Generator)->Activate();   // Start generator at time 0
    Run();                         // Run simulation
    
    // Print results
    Server.Output();
    TimeInSystem.Output();
}
