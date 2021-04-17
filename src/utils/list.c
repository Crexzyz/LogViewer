#include "utils/list.h"

#include <string.h>
#include <stdlib.h>

list_node_t * list_node_create(list_node_t * prev, void * data,
                               size_t data_size)
{
    list_node_t * node = malloc(sizeof(list_node_t));
    list_node_init(node, prev, data, data_size);
    return node;
}

void list_node_init(list_node_t * node, list_node_t * prev,
                    void * data, size_t data_size)
{
    if(!node || !data)
        return;

    node->prev = prev;
    node->next = NULL;
    node->data = malloc(data_size);

    memcpy(node->data, data, data_size);
}

void list_node_destroy(list_node_t * node)
{
    if(node)
    {
        free(node->data);
        free(node);
    }
}

list_t * list_create()
{
    list_t * list = malloc(sizeof(list_t));
    list_init(list);
    return list;
}

void list_init(list_t * list)
{
    if(!list)
        return;
    
    bzero(list, sizeof(list_t));
}

void list_destroy(list_t * list)
{
    if(!list)
        return;

    list_node_t * node = list->first;

    while(node != NULL)
    {
        list_node_t * next = node->next;
        list_node_destroy(node);
        node = next;
    }

    free(list);
}

void list_append(list_t * list, void * data, size_t data_size)
{
    if(!list)
        return;

    list_node_t * node = list_node_create(list->last, data, data_size);

    if(list->size == 0)
    {
        list->first = node;
        list->last = node;
    }
    else
    {
        list->last->next = node;
        list->last = node;
    }

    list->size += 1;
}

list_node_t * list_get_at_index(list_t * list, size_t index)
{
    if(!list || index >= list->size)
        return NULL;

    list_node_t * node = list->first;
    size_t counter = 0;

    while(node != NULL)
    {
        if(counter == index)
            return node;
        
        node = node->next;
        counter += 1;
    }

    return NULL;
}

list_node_t * list_delete(list_t * list, size_t index)
{
    if(!list || index >= list->size)
        return NULL;

    list_node_t * target = list_get_at_index(list, index);

    if(!target)
        return NULL;

    list_node_t * target_prev = target->prev;
    list_node_t * target_next = target->next;

    if(target_prev)
        target_prev->next = target_next;
    else
        list->first = target_next;

    if(target_next)
        target_next->prev = target_prev;
    else
        list->last = target_prev;

    list->size -= 1;

    return target;
}

void list_for_each(list_t * list, void (*func)(void * data))
{
    list_node_t * node = list->first;

    while(node != NULL)
    {
        (*func)(node->data);
        node = node->next;
    }
}

