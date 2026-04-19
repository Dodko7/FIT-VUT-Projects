/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include <stdio.h>
#include "utils/token_types.h"
#include "parser/parser.h"

int main(int argc, char *argv[])
{
    // Initialize the scanner with the source code file.
    scanner = scn_init("../test/scanner/test9.zig");
    if (scanner == NULL) {
        fprintf(stderr, "Failed to initialize scanner.\n");
        return 1;
    }

    // Parse the source code.
    parse();

    // Free the scanner.
    scn_free(scanner);

    return 0;
}