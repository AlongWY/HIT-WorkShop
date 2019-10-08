//
// Created by Along on 2017/5/14.
//

#ifndef GRAPH_BINARYHEAP_H
#define GRAPH_BINARYHEAP_H

typedef struct BinaryHeap *BinaryHeap;

BinaryHeap Heap_create(size_t n);

int Heap_size(BinaryHeap);

void Heap_insert(BinaryHeap, void *key, int (*cmp)(const void *a, const void *b));

void *Heap_delete_key(BinaryHeap, int (*cmp)(const void *a, const void *b));

void Heap_delete(BinaryHeap);


#endif //GRAPH_BINARYHEAP_H
