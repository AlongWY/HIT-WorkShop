//
// Created by Along on 2017/5/25.
//

#include "Queue.h"
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>

typedef struct list {
    int data;              //暂时用int类型吧
    struct list *next;      //下一个
} list;

struct _Queue {
    int n;              //当前容量
    list *node;
    list *end;
};

Queue QueueCreate() {
    Queue queue;
    queue = malloc(sizeof(struct _Queue));
    assert(queue);
    queue->n = 0;
    queue->node = NULL;
    queue->end = NULL;
    return queue;
}

void QueueDelete(Queue q) {
    while (q->n != 0)
        QueuePop(q);
    free(q);
}

void QueuePush(Queue queue, int data) {
    if (queue->n == 0) {
        queue->node = queue->end = malloc(sizeof(list));
        assert(queue->node);
        queue->end->data = data;
    } else {
        queue->end->next = malloc(sizeof(list));
        queue->end = queue->end->next;
        assert(queue->end);
        queue->end->data = data;
    }
    ++queue->n;
}

int QueuePop(Queue queue) {
    int res = queue->node->data;
    list *toFree = queue->node;
    queue->node = toFree->next;
    --queue->n;
    free(toFree);
    return res;
}

int QueueSize(Queue queue) {
    return queue->n;
}
