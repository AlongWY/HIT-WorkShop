//
// Created by Along on 2017/5/25.
//

#include <stdlib.h>
#include "StableMatching.h"
#include "Queue.h"
#include <assert.h>

struct WeightList {
    int n;               //总对数
    Queue single;        //单身汉
    int *mMatch;         //男人匹配中的女士
    int *wMatch;         //女人匹配中的男士
    int *w_man;         //男人心目中的排名
    int *w_woman;       //女人心目中的排名
};

WList WListCreate(int n) {
    WList wList = malloc(sizeof(struct WeightList));
    assert(wList);

    wList->n = n;
    wList->single = QueueCreate();

    wList->mMatch = malloc(sizeof(int) * n);
    wList->wMatch = malloc(sizeof(int) * n);
    wList->w_man = malloc(sizeof(int) * n * n);
    wList->w_woman = malloc(sizeof(int) * n * n);

    assert(wList->mMatch);
    assert(wList->wMatch);
    assert(wList->w_man);
    assert(wList->w_woman);

    int i;
    for (i = 0; i != n; i++) {
        wList->mMatch[i] = -1;
        wList->mMatch[i] = -1;
        QueuePush(wList->single, i);
    }

    return wList;
}

void WlistDelete(WList li) {
    QueueDelete(li->single);
    free(li->w_woman);
    free(li->w_man);
    free(li->mMatch);
    free(li->wMatch);
    free(li);
}

void WListAddMan(WList li, int n, int *Array) {
    assert(n < li->n);
    assert(n >= 0);
    int i;
    for (i = 0; i != li->n; ++i) {
        li->w_man[li->n * n + i] = Array[i];
    }
}

void WListAddWoman(WList li, int n, int *Array) {
    assert(n < li->n);
    assert(n >= 0);
    int i;
    for (i = 0; i != li->n; ++i) {
        li->w_woman[li->n * n + Array[i]] = i;
    }
}

int *Match(WList li) {
    int to_match[li->n];
    while (QueueSize(li->single)) {
        int single = QueuePop(li->single);
//        int toMatch = li->

    }
    return li->mMatch;
}


