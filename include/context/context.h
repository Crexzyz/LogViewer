#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdlib.h>

typedef struct context {
    size_t screen_rows;
    size_t screen_cols;
} context_t;

context_t * context_create();
void context_init(context_t * ctx);
void context_destroy(context_t * ctx);

void context_set_dimensions(context_t * ctx);

#endif // CONTEXT_H