#include "context/context.h"
#include <ncurses.h>
#include <string.h>

context_t * context_create()
{
    context_t * ctx = malloc(sizeof(context_t));
    if(ctx)
        context_init(ctx);
    return ctx;
}

void context_init(context_t * ctx)
{
    bzero(ctx, sizeof(context_t));
}

void context_destroy(context_t * ctx)
{
    if(ctx)
        free(ctx);
}

void context_set_dimensions(context_t * ctx)
{
    getmaxyx(stdscr, ctx->screen_rows, ctx->screen_cols);
}
