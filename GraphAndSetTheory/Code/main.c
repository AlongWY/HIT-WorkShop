#include <stdio.h>

#include "WeightGraph.h"
#include "Graph_tools.h"

int main(int argc, char **argv) {
    WGraph graph = w_graph_create(5);
    w_graph_add_edge2(graph, 0, 1, 100);
    w_graph_add_edge2(graph, 0, 2, 30);
    w_graph_add_edge2(graph, 0, 4, 10);
    w_graph_add_edge2(graph, 1, 2, 50);
    w_graph_add_edge2(graph, 1, 3, 10);
    w_graph_add_edge2(graph, 2, 3, 60);
    w_graph_add_edge2(graph, 3, 4, 50);

    WGraph tree = Kruskal(graph);
    printf("----Source Graph:\n");
    w_graph_show(graph);
    w_graph_show_dot(graph, "Graph.dot");
    printf("----Kruskal Tree:\n");
    w_graph_show(tree);
    w_graph_show_dot(tree, "Kruskal.dot");
    w_graph_destroy(tree);
    printf("----Prim Tree:\n");
    tree = Prim(graph, 0);
    w_graph_show(tree);
    w_graph_show_dot(tree, "Prim.dot");
    w_graph_destroy(tree);
    printf("----The short road:\n");
    Min_len ml = Dijkstra(graph, 0);
    min_len_show(ml);
    min_len_destroy(ml);

    w_graph_destroy(graph);

    return 0;
}