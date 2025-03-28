#ifndef MINIOS_LIST_H
#define MINIOS_LIST_H
#include <os.h>
#include <base/assert.h>

typedef struct list_node
{
    // 上一个节点
    struct list_node* previous;
    // 下一个节点
    struct list_node* next;
} list_node_t;

typedef struct list
{
    // 列表的头部
    list_node_t head;
    // 列表的尾部
    list_node_t tail;
} list_t;

void list_init(list_t* list);

bool list_is_empty(list_t* list);

list_node_t* list_header(list_t* list);

list_node_t* list_tail(list_t* list);

void list_add_header(list_t* list, list_node_t* list_node);

void list_add_tail(list_t* list, list_node_t* list_node);

list_node_t* list_remove_header(list_t* list);

list_node_t* list_remove_tail(list_t* list);

// 移除一个节点
bool list_remove(list_t* list, list_node_t* list_node);

// 将节点 list_node 插入到 anchor_node 的前面
void list_insert_before(list_node_t* anchor_node, list_node_t* list_node);

#endif