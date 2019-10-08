//
// Created by Along on 2017/5/13.
//

#ifndef GRAPH_GRAPH_TOOLS_H
#define GRAPH_GRAPH_TOOLS_H

#include "Graph.h"
#include "WeightGraph.h"

//tools
void w_graph_show(WGraph);

void w_graph_show_dot(WGraph, char path[]);

WGraph w_graph_create_from_file(const char path[]);

void w_graph_add_edge2(WGraph, int source, int sink, int weight);

//求最短路径部分
typedef struct min_len *Min_len;

Min_len Dijkstra(WGraph g, int source);

int min_len_vector_count(Min_len);

void min_len_destroy(Min_len);

void min_len_foreach(Min_len, void(*f)(Min_len, int dist, int prev, void *), void *data);

void min_len_show(Min_len);

//求最小生成树
WGraph Prim(WGraph, int start);

WGraph Kruskal(WGraph);

#endif //GRAPH_GRAPH_TOOLS_H
