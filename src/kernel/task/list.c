#include <task/list.h>

void list_init(list_t* list) {
    list->head.previous = NULL;
    list->tail.next = NULL;
    list->head.next = &list->tail;
    list->tail.previous = &list->head;
}

bool list_is_empty(list_t* list) {
    return (list->head.next == &list->tail);
}

void list_add_header(list_t* list, list_node_t* list_node) {
    assert(list_node != nullptr);
    list_node->next = list->head.next;
    list->head.next->previous = list_node;
    list->head.next = list_node;
    list_node->previous = &list->head;
}

void list_add_tail(list_t* list, list_node_t* list_node) {
    assert(list_node != nullptr);
    list_node->next = &list->tail;
    list->tail.previous->next = list_node;
    list_node->previous = list->tail.previous;
    list->tail.previous = list_node;
}

list_node_t* list_header(list_t* list) {
    assert(list != nullptr);
    if (list_is_empty(list)) {
        return nullptr;
    }
    return list->head.next;
}

list_node_t* list_tail(list_t* list) {
    assert(list != nullptr);
    if (list_is_empty(list)) {
        return nullptr;
    }
    return list->tail.previous;
}

list_node_t* list_remove_header(list_t* list) {
    list_node_t* head = list->head.next;
    list->head.next = head->next;
    head->next->previous = &list->head;
    return head;
}

list_node_t* list_remove_tail(list_t* list) {
    list_node_t* tail = list->tail.previous;
    list->tail.previous = tail->previous;
    tail->previous->next = &list->tail;
    return tail;
}

// 移除一个节点
bool list_remove(list_t* list, list_node_t* list_node){
    assert(list != NULL && list_node != NULL);
    list_node_t* current = list->head.next;
    while (current)
    {
       if (current == list_node) {
            // 找到了
            break;
       }
       // 不断往下找
       current = current->next;
    }
    if (current != list_node) {
        // 没找到
        return false;
    }

    // 移除掉当前节点
    current->previous->next = current->next;
    current->next->previous = current->previous;
    return true;
}