#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct list_node {
    struct list_node * prev;
    struct list_node * next;
    void * data;
} list_node_t;

typedef struct list {
    list_node_t * first;
    list_node_t * last;
    size_t size;
} list_t;

// Node creation
list_node_t * list_node_create(list_node_t * prev, void * data,
                               size_t data_size);

void list_node_init(list_node_t * node, list_node_t * prev,
                    void * data, size_t data_size);

void list_node_destroy(list_node_t * node);

// List creation
list_t * list_create();
void list_init(list_t * list);
void list_destroy(list_t * list);

// Data manipulation
void list_append(list_t * list, void * data, size_t data_size);
list_node_t * list_get_at_index(list_t * list, size_t index);
list_node_t * list_delete(list_t * list, size_t index);

// List iteration
void list_for_each(list_t * list, void (*func)(void * data));


#endif