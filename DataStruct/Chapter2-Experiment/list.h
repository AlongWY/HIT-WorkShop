//
// Created by along on 17-10-23.
//

#ifndef PROJECT_LIST_H
#define PROJECT_LIST_H

typedef void *ElementPtr;

struct _node;
typedef struct _node *PtrNode;
typedef PtrNode List;
typedef PtrNode Position;

/**
 * 创建链表，需要与DeleteList搭配使用
 * @return 创建好的链表指针
 */
List CreateList();

/**
 * 将链表置空，之后不需要重新创建
 * @param l
 * @return
 */
List MakeEmpty(List l);

/**
 * 判断链表是否为空
 * @param l
 * @return
 */
int IsEmpty(List l);

/**
 * 判断该位置是否是链表尾端
 * @param pos
 * @param l
 * @return
 */
int IsLast(Position pos, List l);

/**
 * 查找元素,使用element与链表中的值依次进行比较
 * @param element 要查找的元素值
 * @param l 链表
 * @param cmp 用于比较两个元素的函数指针
 * @return
 */
Position Find(ElementPtr element, List l,
              int (*cmp)(ElementPtr, ElementPtr));

/**
 * 在链表中删除一个元素
 * 注意：所有符合匹配结果的元素都会被删除
 * @param element 要删除的元素
 * @param l
 * @param cmp
 */
void Delete(ElementPtr element, List l,
            int (*cmp)(ElementPtr, ElementPtr));

/**
 * 查找该位置的前一个位置
 * 对于空指针来说，将会返回链表的最后一个节点
 * @param pos
 * @param l
 * @return
 */
Position FindPrevious(Position pos, List l);

/**
 * 向链表中插入一个节点，会插入到该Pos当前所在位置
 * 元素是一个指针，值得注意的是，此元素不需要再次手动Free
 * @param element 要插入的元素指针
 * @param l
 * @param pos
 */
void Insert(ElementPtr element, List l, Position pos);

/**
 * 彻底删除一个链表，之后此链表不可继续使用
 * 内存完全释放版本
 * @param l
 */
void _DeleteList(List l);

/**
 * 彻底删除一个链表，之后此链表不可继续使用
 * 内存可重复使用版本
 * @param l
 * @return NULL
 */
List DeleteList(List l);

/**
 * 返回链表的地一个元素
 * @param l
 * @return
 */
Position First(List l);

/**
 * 返回链表表头，一般不会用到
 * @param l
 * @return
 */
Position Header(List l);

/**
 * 使位置向下移动，得到下一个节点
 * 空节点的后继还是空节点
 * @param pos
 * @return
 */
Position Advance(Position pos);

/**
 * 取回当时存入的指针,之后用户可以对其操作
 * 不可以free，否则极有可能出错
 * @param pos
 * @return
 */
ElementPtr Retrieve(Position pos);

#endif //PROJECT_LIST_H
