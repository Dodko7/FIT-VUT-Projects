#include "../../src/scanner/scanner.h"
#include "../../src/utils/memory_table.h"

int main(int argc, char **argv)
{
	safe_memory = true;
	memory_ht_init(&_memory_table);
	if (argc != 2)
	{
		printf("ERROR: Please fill in file to compile\nUsage: %s <file>\n", argv[0]);
		return 1;
	}

	Scanner_ptr scanner = scn_init(argv[1]);
	size_t i = 0;
	Token_ptr element = NULL;
	do
	{
		if (element != NULL)
			tok_free(element);
		element = scn_scan(scanner);
		printf("%4d | %-20s | %2d |%-14s |\t%s\n", i++, tok_type_to_str(element->type), element->type, kw_get_lexeme(element->type) != NULL ? kw_get_lexeme(element->type) : "", element->attribute != NULL ? element->attribute : "");
		fflush(NULL);
	} while (element->type != tok_t_eof && element->type != tok_t_error);
	tok_free(element);
	scn_free(scanner);
	memory_ht_dispose(&_memory_table);
	return 0;
}