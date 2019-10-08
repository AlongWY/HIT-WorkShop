//
// Created by Along on 2017/5/13.
//

#ifndef GRAPH_WEIGHTGRAPH_H
#define GRAPH_WEIGHTGRAPH_H

#include "Graph.h"

//不可达时的返回值
#define INFINITY (65535)

typedef struct w_graph *WGraph;

WGraph w_graph_create(int n);

void w_graph_destroy(WGraph);

void w_graph_add_edge(WGraph, int source, int sink, int weight);

int w_graph_vector_count(WGraph);

int w_graph_edge_count(WGraph);

int w_graph_out_degree(WGraph, int source);

int w_graph_has_edge(WGraph, int source, int sink);

int w_graph_weight_edge(WGraph, int source, int sink);

void w_graph_foreach(WGraph g, int source,
                     void (*f)(WGraph, int src, int sink, int weight, void *),
                     void *data);

#endif //GRAPH_WEIGHTGRAPH_H
