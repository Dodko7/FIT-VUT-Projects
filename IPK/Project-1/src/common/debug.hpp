#ifndef DEBUG_HPP
#define DEBUG_HPP

// Debug logging macro that prints to stderr when DEBUG_PRINT is defined.
#ifdef DEBUG_PRINT
#define printf_debug(format, ...) fprintf(stderr, "[%s:%d:%s] " format "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define printf_debug(format, ...) ((void)0)
#endif

#endif // DEBUG_HPP