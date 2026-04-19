#include "../../src/scanner/scanner.h"

int main()
{
	printf("This test relies on file test_com_str.zig, please make sure\nyou are launching this from the project root and not ./build/ for example\n");
	Scanner_ptr scanner = scn_init("./test/scanner/test_com_str.zig");
	/*
	test/scanner/test0.zig program in tokens

	const ifj = @import("ifj24.zig");

	pub fn bar(param : []u8) []u8 {
		const r = foo(param);
		return r;
	}

	pub fn foo(par : []u8) []u8 {
		const ret = bar(par);
		return ret;
	}

	pub fn main() void {
		const par = ifj.string("ahoj");
		_ = bar(par);
	}
	 */
	Token_ptr token[] = {
		// line 1
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here ifj
		tok_init(tok_t_ass),
		tok_init(tok_t_import),
		tok_init(tok_t_lpa),
		tok_init(tok_t_str), // here ifj24.zig
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		// line 2
		// line 3
		/*
		pub fn bar(param : []u8) []u8 {
			const r = foo(param);
			return r;
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here param
		tok_init(tok_t_colon),
		tok_init(tok_t_u8),
		tok_init(tok_t_rpa),
		tok_init(tok_t_u8),
		tok_init(tok_t_lcbr),
		// line 4
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here r
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here foo
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here param
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		// line 5
		tok_init(tok_t_return),
		tok_init(tok_t_sym), // here r
		tok_init(tok_t_semicolon),
		// line 6
		tok_init(tok_t_rcbr),
		// line 7
		// line 8
		/*
		pub fn foo(par : []u8) []u8 {
			const ret = bar(par);
			return ret;
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here foo
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_colon),
		tok_init(tok_t_u8),
		tok_init(tok_t_rpa),
		tok_init(tok_t_u8),
		tok_init(tok_t_lcbr),
		// line 9
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here ret
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		// line 10
		tok_init(tok_t_return),
		tok_init(tok_t_sym), // here ret
		tok_init(tok_t_semicolon),
		// line 11
		tok_init(tok_t_rcbr),
		// line 12
		// line 13
		/*
		pub fn main() void {
			const par = ifj.string("ahoj");
			_ = bar(par);
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here main
		tok_init(tok_t_lpa),
		tok_init(tok_t_rpa),
		tok_init(tok_t_void),
		tok_init(tok_t_lcbr),
		// line 14
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here ifj
		tok_init(tok_t_dot),
		tok_init(tok_t_sym), // here string
		tok_init(tok_t_lpa),
		tok_init(tok_t_str), // here ahoj
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		// line 15
		tok_init(tok_t_unused), // here _
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		// line 15
		tok_init(tok_t_rcbr),
		tok_init(tok_t_eof)};

	tok_set_attribute(token[1], "ifj");
	tok_set_attribute(token[5], "ifj24.zig");
	tok_set_attribute(token[10], "bar");
	tok_set_attribute(token[12], "param");

	tok_set_attribute(token[19], "r");
	tok_set_attribute(token[21], "foo");
	tok_set_attribute(token[23], "param");

	tok_set_attribute(token[27], "r");



	tok_set_attribute(token[32], "foo");
	tok_set_attribute(token[34], "par");

	tok_set_attribute(token[41], "ret");
	tok_set_attribute(token[43], "bar");
	tok_set_attribute(token[45], "par");
	
	tok_set_attribute(token[49], "ret");



	tok_set_attribute(token[54], "main");

	tok_set_attribute(token[60], "par");
	tok_set_attribute(token[62], "ifj");
	tok_set_attribute(token[64], "string");
	tok_set_attribute(token[66], "ahoj");

	tok_set_attribute(token[69], "_");
	tok_set_attribute(token[71], "bar");
	tok_set_attribute(token[73], "par");
	for (int i = 0; i < 78; i++)
	{
		tok_dll_push_back(scanner->list, *token[i]);
		tok_free(token[i]);
	}
	tok_dll_first(scanner->list);
	size_t i = 0;
	while (scanner->list->activeElement != NULL)
	{
		Token_ptr element = scn_next(scanner);
		printf("%4ld | %14s |\t%s\n", i++, kw_get_lexeme(element->type) != NULL ? kw_get_lexeme(element->type) : "", element->attribute != NULL ? element->attribute : "");
	}
	scn_free(scanner);
	return 0;
}