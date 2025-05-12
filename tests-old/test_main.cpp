#define CATCH_CONFIG_RUNNER  // Use custom runner
#include <catch.hpp>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    // Create QCoreApplication before running tests
    QCoreApplication app(argc, argv);
    
    // Run tests with Catch2
    int result = Catch::Session().run(argc, argv);
    
    // Return test result
    return result;
}