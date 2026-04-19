cmake_minimum_required(VERSION 3.10)
project(ifj2024)
set(CMAKE_C_STANDARD 99)
include (CTest)

set(VALGRIND_OK ".*All heap blocks were freed -- no leaks are possible.*")
set(CMAKE_BUILD_TYPE Debug)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(UTILS ${SRC}/utils)
set(SCANNER ${SRC}/scanner)
set(PARSER ${SRC}/parser)

add_executable(ifj2024 src/main.c)

add_library(errors SHARED    	 ${UTILS}/errors.c)
add_library(token SHARED    	 ${UTILS}/token.c)
add_library(keyword SHARED 	 	 ${UTILS}/token_types.c)
add_library(sll SHARED    	     ${UTILS}/sll.c)
add_library(dll SHARED    	     ${UTILS}/dll.c)
add_library(dll_trad SHARED    	 ${UTILS}/dll.c)
add_library(scan_state SHARED	 ${SCANNER}/scan_state.c)
add_library(scanner SHARED    	 ${SCANNER}/scanner.c)
add_library(token_dll SHARED 	 ${UTILS}/token_dll.c)
add_library(sym_dll SHARED 	 	 ${UTILS}/symDLList.c)
add_library(dym_string SHARED 	 ${UTILS}/dymString.c)
add_library(parser SHARED 	 	 ${PARSER}/parser.c)
add_library(stack SHARED 	     ${UTILS}/stack.c)
add_library(symtable SHARED 	 ${UTILS}/symtable.c)
add_library(codegen SHARED 	     ${UTILS}/codegen.c)
add_library(str SHARED 	         ${UTILS}/str.c)
add_library(hashtable SHARED 	 ${UTILS}/hashtable.c)
add_library(memory_table SHARED  ${UTILS}/memory_table.c)
target_compile_definitions(dll_trad PUBLIC DLL_TRAD)

target_link_libraries(dll memory_table)
target_link_libraries(sll memory_table)
target_link_libraries(token memory_table)
target_link_libraries(keyword errors)
target_link_libraries(scanner token errors scan_state keyword token_dll memory_table m)
target_link_libraries(ifj2024 errors token scanner parser codegen stack symtable sym_dll dym_string str)



# Enable testing
enable_testing()

# tests
# dll
add_executable(test_dll_1 ${TEST}/dll/DLL_test_1.c)
add_executable(test_dll_2 ${TEST}/dll/DLL_test_2.c)
add_executable(test_dll_3 ${TEST}/dll/DLL_test_3.c)
add_executable(test_dll_4 ${TEST}/dll/DLL_test_4.c)
add_executable(test_trad_dll_0 ${TEST}/dll/DLL_trad_test_0.c)
# sll
add_executable(test_sll_1 ${TEST}/sll/SLL_test_1.c)
# hashtable
add_executable(test_ht_0 ${TEST}/hashtable/test_hash_0.c)
add_executable(test_ht_1 ${TEST}/hashtable/test_hash_1.c)
add_executable(test_ht_2 ${TEST}/hashtable/test_hash_2.c)
add_executable(test_memory_table ${TEST}/hashtable/test_memory_table.c)
# scanner
add_executable(test_scan_1 ${TEST}/scanner/scan_1.c)
add_executable(test_scan_2 ${TEST}/scanner/scan_2.c)
add_executable(test_scan_3 ${TEST}/scanner/scan_3.c)
add_executable(test_scan_4 ${TEST}/scanner/scan_4.c)

target_link_libraries(test_memory_table memory_table token keyword)
target_link_libraries(test_ht_0 sll)
target_link_libraries(test_ht_1 sll)
target_link_libraries(test_ht_2 sll)
target_link_libraries(test_dll_1 memory_table)
target_link_libraries(test_dll_2 memory_table)
target_link_libraries(test_dll_3 memory_table)

target_link_libraries(test_dll_4 token dll keyword)
target_link_libraries(test_sll_1 sll)
target_link_libraries(test_scan_1 token dll keyword scanner)
target_link_libraries(test_scan_2 token dll keyword scanner)
target_link_libraries(test_scan_3 token dll keyword scanner)
target_link_libraries(test_scan_4 token dll keyword scanner)
target_link_libraries(test_trad_dll_0 token dll_trad keyword scanner)

# test covering initialization, push_front and clear in DLL macro
add_test(NAME "dll-init" COMMAND test_dll_1)
set_property(TEST "dll-init"
              PROPERTY PASS_REGULAR_EXPRESSION ".*11 10 9 8 7 42 6 5 4 3 2 1.*")

add_test(NAME "dll-init-valgrind" COMMAND valgrind ./test_dll_1)
set_property(TEST "dll-init-valgrind"
              PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering pushes, pops and deletion on specific index in DLL macro
add_test(NAME "dll-push-pop-delete" COMMAND test_dll_2)
set_property(TEST "dll-push-pop-delete"
              PROPERTY PASS_REGULAR_EXPRESSION "1 2 3 4 5 42 6 7 8 9 10.*
.*2 3 4 42 6 7 8 9.*")

add_test(NAME "dll-push-pop-delete-valgrind" COMMAND valgrind ./test_dll_2)
set_property(TEST "dll-push-pop-delete-valgrind"
              PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering DLL with strings
add_test(NAME "dll-string" COMMAND test_dll_3)
set_property(TEST "dll-string"
              PROPERTY PASS_REGULAR_EXPRESSION "Lorem ipsum dolor sit amet incididunt consectetur adipiscing elit sed do eiusmod tempor")

add_test(NAME "dll-string-valgrind" COMMAND valgrind ./test_dll_2)
set_property(TEST "dll-string-valgrind"
              PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering DLL with custom structure
add_test(NAME "dll-token" COMMAND test_dll_4)
set_property(TEST "dll-token"
              PROPERTY PASS_REGULAR_EXPRESSION ".*const else fn if i32 while f64 null pub return \\[\\]u8 void while.*")

add_test(NAME "dll-token-valgrind" COMMAND valgrind ./test_dll_4)
set_property (TEST "dll-token-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering traditional DLL with custom structure
add_test(NAME "dll-trad-token" COMMAND test_trad_dll_0)
set_property (TEST "dll-trad-token"
              PROPERTY PASS_REGULAR_EXPRESSION ".*const else fn if i32 while f64 null pub return \\[\\]u8 void while.*")

add_test(NAME "dll-trad-token-valgrind" COMMAND valgrind ./test_trad_dll_0)
set_property (TEST "dll-trad-token-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering SLL with int
add_test(NAME "sll-int" COMMAND test_sll_1)
set_property (TEST "sll-int" PROPERTY PASS_REGULAR_EXPRESSION ".*11 10 9 8 7 42 6 5 4 3 2 1.*")

add_test(NAME "sll-int-valgrind" COMMAND valgrind ./test_sll_1)
set_property (TEST "sll-int-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering HT with int
add_test(NAME "ht-int" COMMAND test_ht_0)
set_property (TEST "ht-int" PROPERTY PASS_REGULAR_EXPRESSION ".*0: _33__22__11__0_1: _34__23__12__1_2: _35__24__13__2_3: _36__25__14__3_4: _37__26__15__4_5: _38__27__16__5_6: _28__17__6_7: _29__18__7_8: _30__19__8_9: _31__20__9_10: _32__21__10_.*")

add_test(NAME "ht-int-valgrind" COMMAND valgrind ./test_ht_0)
set_property (TEST "ht-int-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# scanner tests
foreach(test "float_bad" "float" "strings_bad" "strings" "keywords" "full" "ints")
	add_test(NAME "scanner-${test}" COMMAND sh -c "./test_scan_3 \"${CMAKE_SOURCE_DIR}/test/scanner/atomic_expressions/${test}.txt\" > \"${test}.out\" && cmake -E compare_files \"${CMAKE_SOURCE_DIR}/test/scanner/atomic_expressions/${test}.test\" \"${test}.out\"" )
    # add_test(NAME "scanner-${test}" COMMAND cmake -E compare_files "${CMAKE_SOURCE_DIR}/test/scanner/atomic_expressions/${test}.test" "${test}.out")
    set_property(TEST "scanner-${test}"
              PROPERTY SKIP_RETURN_CODE)
    add_test(NAME "scanner-${test}-valgrind" COMMAND valgrind ./test_scan_3 "${CMAKE_SOURCE_DIR}/test/scanner/atomic_expressions/${test}.txt")
    set_property(TEST "scanner-${test}-valgrind"
              PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})
endforeach()