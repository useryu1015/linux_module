#include <stdio.h>
/* buffer */
typedef struct _enum_map_t {
	int id;
	char name[32];
    int (*run_func)(void *);
} enum_map_t;


/*
 * Insert user code */
typedef enum {
    COLOR_RED, 
    COLOR_GREEN, 
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
} color_type;
/* Insert end */


/*
 * Output code */
/* Output end */
