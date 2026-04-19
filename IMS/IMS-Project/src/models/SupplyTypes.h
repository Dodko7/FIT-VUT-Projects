#ifndef SUPPLY_TYPES_H
#define SUPPLY_TYPES_H

enum SupplyType {
    GRAIN,           // Wheat/Barley combined
    FODDER,          // Animal fodder
    SALTED_MEAT,     // Salted meat
    WINE_OIL,        // Wine/Oil  
    EQUIPMENT,       // Equipment/Clothing
    NUM_SUPPLY_TYPES
};

extern const char* SupplyNames[];

#endif // SUPPLY_TYPES_H
