/**
 * @file main.cpp
 * @brief Main entry point for DNS filter server
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-09-10
 */

#include "../include/common.h"
#include "../include/arg_parser.h"
#include "../include/logger.h"
#include "../include/filter.h"
#include "../include/server.h"
#include <iostream>

// ============================================================================
// Main Function
// ============================================================================
int main(int argc, char* argv[]) {
    // ==================================================
    // STEP 1: Parse Command-Line Arguments
    // ==================================================
    Config config = parse_arguments(argc, argv);
    //       │
    //       ├─▶ Parse -s, -p, -f, -v flags using getopt()
    //       ├─▶ Validate required args
    //       └─▶ Resolve server address to sockaddr_storage
    
    // Check if parsing was successful
    if (!config.valid) {
        LOG_ERROR("Failed to parse arguments");
        return EXIT_FAILURE_CODE;  // Exit with error code 1
    }

    
    // ==================================================
    // STEP 2: Load Filter File
    // ==================================================
    DomainFilter filter;
    if (!filter.load_filter_file(config.filter_file)) {
        //                    │
        //                    ├─▶ Read file line by line
        //                    ├─▶ Normalize and validate domains (RFC compliance)
        //                    └─▶ Store in hash table (std::unordered_set)
        
        LOG_ERROR("Failed to load filter file");
        return EXIT_FAILURE_CODE;
    }
    
    LOG_INFO_F("Domain filter initialized: %zu blocked domains", 
                filter.get_domain_count());
    

    // ==================================================
    // STEP 3: Start DNS Server
    // ==================================================
    int server_result = run_dns_server(config, filter);
    //                      │
    //                      ├─▶ Create sockets and bind to port
    //                      ├─▶ poll() event loop for client/upstream events
    //                      └─▶ Process queries: parse, filter, forward, respond
    
    // Check server result
    if (server_result != EXIT_SUCCESS_CODE) {
        LOG_ERROR("DNS server terminated with error");
        return EXIT_FAILURE_CODE;
    }
    
    LOG_INFO("DNS filter server shutdown complete");
    return EXIT_SUCCESS_CODE;
}