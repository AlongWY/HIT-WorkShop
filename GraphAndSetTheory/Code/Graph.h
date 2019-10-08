//
// Created by Along on 2017/5/13.
//

#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

typedef struct graph *Graph;

Graph graph_create(int n);

void graph_destroy(Graph);

void graph_add_edge(Graph, int source, int sink);

int graph_vertex_count(Graph);

int graph_edge_count(Graph);

int graph_out_degree(Graph, int source);

int graph_has_edge(Graph, int source, int sink);

void graph_foreach(Graph g, int source,
                   void (*f)(Graph, int, int, void *),
                   void *data);


#endif //GRAPH_GRAPH_H
