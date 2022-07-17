#include "list.h"


List *List_push(void *item, List *list) {
    List *list_ = malloc(sizeof(List));
    list_->n = list ? list->n + 1 : 1;
    list_->item = item;
    list_->tail = list;
    return list_;
}

List *List_pop(List *list) {
    List *tail = list->tail;
    free(list);
    return tail;
}

void List_free(List *list) {
    List *current = list, *tail;
    while (current && (tail = current->tail)) {
        free(current);
        current = tail;
    }
}

size_t List_size(List *list) {
    return list ? list->n : 0;
}