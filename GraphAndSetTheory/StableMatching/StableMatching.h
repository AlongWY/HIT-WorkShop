//
// Created by Along on 2017/5/25.
//

#ifndef GRAPH_STABLEMATCHING_H
#define GRAPH_STABLEMATCHING_H

typedef struct WeightList *WList;

//创建排名列表,共有n个人
WList WListCreate(int n);

void WlistDelete(WList);

//输入第n个男人的权值表
void WListAddMan(WList, int n, int *);

//输入第n个女人的权值表
void WListAddWoman(WList, int n, int *);

//进行匹配
int *Match(WList);

#endif //GRAPH_STABLEMATCHING_H
