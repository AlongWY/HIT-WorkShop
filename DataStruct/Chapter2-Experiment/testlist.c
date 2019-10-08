//
// Created by along on 17-10-23.
//

#include <stdio.h>
#include "list.h"
#include <malloc.h>

int cmp(ElementPtr first, ElementPtr second) {
    return *(int *) first - *(int *) second;
}

int main() {
    List list = CreateList();

    for (int i = 0; i != 10; ++i) {
        int *element = (int *) malloc(sizeof(int));
        *element = i;
        Insert(element, list, First(list));
    }

    for (int i = 0; i != 20; ++i) {
        Position pos = Find(&i, list, cmp);
        Delete(pos, list, cmp);
    }

    for (int i = 0; i != 30; ++i) {
        int *element = (int *) malloc(sizeof(int));
        *element = i;
        Insert(element, list, First(list));
    }

    MakeEmpty(list);

    for (int i = 0; i != 40; ++i) {
        int *element = (int *) malloc(sizeof(int));
        *element = i;
        Insert(element, list, First(list));
    }

    MakeEmpty(list);
    DeleteList(list);
    return 0;
}