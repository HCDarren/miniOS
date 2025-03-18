#include <task/list.h>

void list_init(list_t* list) {
    list->head = list->tail = nullptr;
}

bool list_is_empty(list_t* list) {
    return list->head == nullptr;
}

void list_add_header(list_t* list, list_node_t* list_node) {
    assert(list_node != nullptr);
    list_node->previous = list_node->next = nullptr;
    if (list_is_empty(list)) {
        list->head = list->tail = list_node;
        return;
    }
    list_node->next = list->head;
    list->head->previous = list_node;
    list->head = list_node;
}

void list_add_tail(list_t* list, list_node_t* list_node) {
    assert(list_node != nullptr);
    if (list_is_empty(list)) {
        list->head = list->tail = list_node;
        return;
    }
    list_node->next = nullptr;
    list_node->previous = list->tail;
    list->tail->next = list_node;
    list->tail = list_node;
}

list_node_t* list_header(list_t* list) {
    assert(list != nullptr);
    return list->head;
}

list_node_t* list_tail(list_t* list) {
    assert(list != nullptr);
    return list->tail;
}

list_node_t* list_remove_header(list_t* list) {
    if (list_is_empty(list)) {
        return nullptr;
    }
    list_node_t* head = list->head;
    list->head = head->next;
    head->next = head->previous = nullptr;
    return head;
}

list_node_t* list_remove_tail(list_t* list) {
    if (list_is_empty(list)) {
        return nullptr;
    }
    list_node_t* tail = list->tail;
    list->tail = tail->previous;
    tail->next = tail->previous = nullptr;
    return tail;
}