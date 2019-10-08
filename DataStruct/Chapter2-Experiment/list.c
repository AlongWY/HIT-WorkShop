//
// Created by along on 17-10-23.
//

#include <malloc.h>
#include <stdlib.h>
#include "list.h"

struct _node {
    ElementPtr element;
    struct _node *next;
};

static List AvaList = NULL;

void DeleteNode(PtrNode node) {
    // 将element域的内存释放并入AvaList
    node->next = First(AvaList);
    free(node->element);
    AvaList->next = node;
}

void FreeMem() {
    // 将可用空间表内存全部释放
    _DeleteList(AvaList);
}

static PtrNode GetAva() {
    // 如果为空就分配内存，否则返回可用空间
    if (AvaList == NULL) {
        AvaList = CreateList();
        //程序终止时完全释放内存
        atexit(FreeMem);
    }
    PtrNode node = First(AvaList);
    if (node != NULL)
        AvaList->next = node->next;
    else
        node = (PtrNode) malloc(sizeof(struct _node));
    return node;
}

List MakeEmpty(List l) {
    // 将链表节点全部送入可用空间表
    while (!IsEmpty(l)) {
        PtrNode node = First(l)->next;
        DeleteNode(First(l));
        l->next = node;
    }
    return l;
}

int IsEmpty(List l) {
    //判断链表是否为空
    if (l == NULL)
        return 1;
    return l->next == NULL;
}

int IsLast(Position pos, List l) {
    //判断pos是不是list的最后一个元素
    return pos->next == NULL;
}

Position Find(ElementPtr element, List l,
              int (*cmp)(ElementPtr, ElementPtr)) {
    if (element == NULL)
        return NULL;
    Position pos = First(l);
    // 从头遍历，如果找不到，最后返回的是空指针
    while (pos != NULL && cmp(element, pos->element) != 0)
        pos = Advance(pos);
    return pos;
}

void Delete(ElementPtr element, List l,
            int (*cmp)(ElementPtr, ElementPtr)) {
    // 所有找到的都删除
    for (Position pos = Find(element, l, cmp); pos != NULL;
         pos = Find(element, l, cmp)) {
        Position pre = FindPrevious(pos, l);
        pre->next = Advance(pos);
        DeleteNode(pos);
    }
}

Position FindPrevious(Position pos, List l) {
    //pos的前驱位置
    Position p = First(l);
    if (pos == p)
        return l;
    while (Advance(p) != pos && p != NULL)
        p = Advance(p);
    return p;
}

void Insert(ElementPtr element, List l, Position pos) {
    //将节点插入链表
    Position pre = FindPrevious(pos, l);
    if (pos == NULL)
        pos = pre->next;
    pre->next = GetAva();
    pre = Advance(pre);
    pre->element = element;
    pre->next = pos;
}

void _DeleteList(List l) {
    PtrNode header = First(l);
    // 遍历释放内存
    while (!IsEmpty(l)) {
        PtrNode ptrNode = header;
        if (ptrNode == NULL) {
            if (l != NULL)
                free(l);
            return;
        }
        header = Advance(header);
        free(ptrNode);
    }
    free(l);
}

Position First(List l) {
    //返回链表的首元素
    if (l == NULL)
        return NULL;
    return l->next;
}

Position Advance(Position pos) {
    //pos的后继节点
    if (pos == NULL)
        return NULL;
    return pos->next;
}

ElementPtr Retrieve(Position pos) {
    //取出节点的信息域元素
    if (pos == NULL)
        return NULL;
    return pos->element;
}

List CreateList() {
    //创建链表
    List list = (List) malloc(sizeof(struct _node));
    list->element = NULL;
    list->next = NULL;
    return list;
}

Position Header(List l) {
    //返回链表头节点
    return l;
}

List DeleteList(List l) {
    //删除链表，返还内存给可用空间表
    MakeEmpty(l);
    free(l);
    return NULL;
}

