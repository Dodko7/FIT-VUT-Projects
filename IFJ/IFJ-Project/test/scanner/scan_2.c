#include "../../src/scanner/scanner.h"

int main()
{
	printf("This test relies on file test_com_str.zig, please make sure\nyou are launching this from the project root and not ./build/ for example\n");
	Scanner_ptr scanner = scn_init("./test/scanner/test_com_str.zig");
	size_t i = 0;
	Token_ptr element = NULL;
	do {
		element = scn_scan(scanner);
	} while (element->type != tok_t_eof);
	scn_free(scanner);
	return 0;
}