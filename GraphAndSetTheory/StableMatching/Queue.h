//
// Created by Along on 2017/5/25.
//

#ifndef GRAPH_QUEUE_H
#define GRAPH_QUEUE_H

typedef struct _Queue *Queue;

Queue QueueCreate();

void QueueDelete(Queue);

void QueuePush(Queue, int data);

int QueuePop(Queue);

int QueueSize(Queue);

#endif //GRAPH_QUEUE_H
