#include "tab.h"

#include <stdlib.h>

tab_t * tab_create()
{
    tab_t * tab = malloc(sizeof(tab_t));
    tab_init(tab);
    return tab;
}

void tab_init(tab_t * tab)
{
    if(tab) 
        bzero(tab, sizeof(tab_t));
}

void tab_destroy(tab_t * tab)
{
    if(tab)
        free(tab);
}