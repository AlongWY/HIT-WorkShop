//
// Created by Along on 2017/5/14.
//

#include <stddef.h>
#include <malloc.h>
#include <assert.h>
#include "Binaryheap.h"

struct BinaryHeap {
    int n;
    int len;
    void *data[1];
};

BinaryHeap Heap_create(size_t n) {
    BinaryHeap h = malloc(n * sizeof(BinaryHeap) + sizeof(void *) * (n - 1));
    assert(h);
    h->n = 0;
    h->len = (int) n;
    return h;
}

void Heap_insert(BinaryHeap h, void *key, int (*cmp)(const void *, const void *)) {
    assert(key);
    //动态扩展堆大小
    if (h->n >= h->len) {
        h->len *= 2;
        h = realloc(h, sizeof(BinaryHeap) + sizeof(void *) * (h->len - 1));
    }

    //上滤
    int hole = ++h->n;
    void *copy = key;

    h->data[0] = copy;
    for (; cmp(key, h->data[hole / 2]) < 0; hole /= 2) {
        h->data[hole] = h->data[hole / 2];
    }
    h->data[hole] = h->data[0];
    h->data[0] = NULL;
}

static void percolateDown(BinaryHeap h, int hole, int (*cmp)(const void *, const void *)) {
    int child;
    void *tmp = h->data[hole];
    for (; hole * 2 <= h->n; hole = child) {
        child = hole * 2;
        if (child != h->n && cmp(h->data[child + 1], h->data[child]) < 0)
            ++child;
        if (cmp(h->data[child], tmp) < 0)
            h->data[hole] = h->data[child];
        else
            break;
    }
    h->data[hole] = tmp;
}


void *Heap_delete_key(BinaryHeap h, int (*cmp)(const void *, const void *)) {
    assert(h->n > 0);
    void *res = h->data[1];

    h->data[1] = h->data[h->n--];
    percolateDown(h, 1, cmp);
    return res;
}

int Heap_size(BinaryHeap h) {
    return h->n;
}

void Heap_delete(BinaryHeap h) {
    free(h);
}




