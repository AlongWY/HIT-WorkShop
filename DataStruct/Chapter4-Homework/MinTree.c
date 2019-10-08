#include <stdio.h>

#include "WeightGraph.h"
#include "Graph_tools.h"

int main(int argc, char **argv) {
    const char *filename = "Graph.txt";
    WGraph graph = w_graph_create_from_file(filename);

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

    w_graph_destroy(graph);

    return 0;
}