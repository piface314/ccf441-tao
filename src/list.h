#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct list {
    size_t n;
    void *item;
    struct list *tail;
} List;

List *List_push(void *item, List *list);
void List_free(List *list);
size_t List_size(List *list);

#endif