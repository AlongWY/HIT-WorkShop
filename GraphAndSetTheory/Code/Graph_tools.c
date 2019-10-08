//
// Created by Along on 2017/5/13.
//

#include <stddef.h>
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <regex.h>
//#include "Graph.h"
#include "WeightGraph.h"
#include "Graph_tools.h"
#include "Binaryheap.h"

struct min_len {
    int n;
    int vec;
    struct list {
        int dist;           //与所要求顶点的距离
        int prev;           //前驱动点
    } a_list[1];
};

Min_len Dijkstra(WGraph g, int source) {
    int i, j, *S;
    Min_len res;

    int vec_num = w_graph_vector_count(g);

    assert(source >= 0);
    assert(source < vec_num);

    res = malloc(sizeof(struct min_len) + sizeof(struct list) * (vec_num - 1));
    S = calloc((size_t) vec_num, sizeof(int));
    assert(res);
    assert(S);

    res->n = vec_num;
    res->vec = source;

    //初始化各点
    for (i = 0; i != vec_num; ++i) {
        S[i] = 0;
        if (w_graph_has_edge(g, source, i)) {
            res->a_list[i].dist = w_graph_weight_edge(g, source, i);
            res->a_list[i].prev = source;
        } else {
            res->a_list[i].prev = -1;
            res->a_list[i].dist = INFINITY;
        }
    }

    res->a_list[source].dist = 0;
    res->a_list[source].prev = source;
    S[source] = 1;

    for (i = 1; i != vec_num; ++i) {
        int min_dst = INFINITY;
        int u = source;
        //找出未使用过的点中dist最小的
        for (j = 0; j != vec_num; ++j) {
            if ((!S[j]) && res->a_list[j].dist < min_dst) {
                u = j;                                          //u是距离source最近的点
                min_dst = res->a_list[j].dist;
            }
        }

        S[u] = 1;                                               //将u标记为已使用

        for (j = 0; j != vec_num; ++j)
            if ((!S[j]) && w_graph_has_edge(g, u, j)) {         //j点未被使用且u,j之间有边
                if (res->a_list[u].dist + w_graph_weight_edge(g, u, j) < res->a_list[j].dist) {
                    res->a_list[j].dist = res->a_list[u].dist + w_graph_weight_edge(g, u, j);   //更新距离
                    res->a_list[j].prev = u;                                                    //更新路径
                }
            }
    }
    free(S);
    return res;
}

void min_len_foreach(Min_len m, void (*f)(Min_len, int, int, void *), void *data) {
    int i;
    for (i = 0; i != m->n; i++) {
        f(m, m->a_list[i].dist, m->a_list[i].prev, data);
    }
}

int min_len_vector_count(Min_len m) {
    return m->n;
}

void min_len_destroy(Min_len m) {
    free(m);
}

void w_graph_add_edge2(WGraph g, int source, int sink, int weight) {
    w_graph_add_edge(g, source, sink, weight);
    w_graph_add_edge(g, sink, source, weight);
}

static void w_graph_show_vec(WGraph g, int src, int sink, int weight, void *data) {
    printf(" %d:%d ", sink, weight);
}

void w_graph_show(WGraph g) {
    int i;
    for (i = 0; i != w_graph_vector_count(g); ++i) {
        printf("%d:", i);
        w_graph_foreach(g, i, w_graph_show_vec, NULL);
        putchar('\n');
    }
}

void min_len_show(Min_len ml) {
    int i, j;
    for (i = 0; i != ml->n; ++i) {
        printf("Dist:%d ", ml->a_list[i].dist);
        for (j = i; j != ml->vec; j = ml->a_list[j].prev) {
            printf("%d <- ", j);
        }
        printf("%d\n", ml->vec);
    }
}

//辅助工具
//边遍历工具
struct need_data {
    int *near;
    int *Len;
    int *S;
};

static void update_edge(WGraph g, int src, int sink, int weight, void *data) {
    assert(g);
    if (!((struct need_data *) data)->S[sink]) {
        if (weight < ((struct need_data *) data)->Len[sink]) {
            ((struct need_data *) data)->Len[sink] = weight;
            ((struct need_data *) data)->near[sink] = src;
        }
    }
}

WGraph Prim(WGraph g, int start) {
    int i, j;
    int vec_num = w_graph_vector_count(g);
    WGraph res = w_graph_create(vec_num);
    assert(res);
    assert(start >= 0);
    assert(start < vec_num);

    struct need_data Data;
    Data.S = calloc((size_t) vec_num, sizeof(int));     //逐步增加的新顶点集
    Data.Len = calloc((size_t) vec_num, sizeof(int));   //到树的最小边
    Data.near = calloc((size_t) vec_num, sizeof(int));  //最近临接顶点
    for (i = 0; i != vec_num; ++i)
        Data.Len[i] = INFINITY;
    Data.S[start] = 1;
    Data.Len[start] = 0;
    int curr = start;

    for (i = 1; i != vec_num; ++i) {
        w_graph_foreach(g, curr, update_edge, &Data);   //通过curr更新各边最短值
        int near_len = INFINITY;
        for (j = 0; j != vec_num; ++j) {
            if (!Data.S[j] && Data.Len[j] < near_len) {
                near_len = Data.Len[j];
                curr = j;
            }
        }
        Data.S[curr] = 1;
        w_graph_add_edge2(res, curr, Data.near[curr], Data.Len[curr]);
    }
    free(Data.near);
    free(Data.Len);
    free(Data.S);
    return res;
}

typedef struct _Edge {
    int from;
    int to;
    int len;
} Edge;

int edge_cmp(const void *a, const void *b) {
    return ((Edge *) a)->len - ((Edge *) b)->len;
}


static void insertEdge(WGraph g, int src, int sink, int weight, void *heap) {
    assert(g);
    Edge *edge = malloc(sizeof(Edge));
    edge->from = src;
    edge->to = sink;
    edge->len = weight;
    Heap_insert((BinaryHeap) heap, edge, edge_cmp);
}

//并查集优化版
WGraph Kruskal(WGraph g) {
    int i, SetType;
    int vec_num = w_graph_vector_count(g);
    WGraph res = w_graph_create(vec_num);
    BinaryHeap heap = Heap_create((size_t) w_graph_edge_count(g));
    int *S = calloc((size_t) vec_num, sizeof(int));
    assert(S);

    for (i = 0; i != vec_num; ++i) {
        w_graph_foreach(g, i, insertEdge, heap);
        S[i] = i;                                       //森林
    }
    Edge *e = NULL;

    while (w_graph_vector_count(res) != (w_graph_edge_count(res) / 2 + 1)) {
        e = Heap_delete_key(heap, edge_cmp);
        //如果加入这条边不会形成圈
        if (S[e->from] != S[e->to]) {
            //接收此边并合并
            w_graph_add_edge2(res, e->from, e->to, e->len);
            SetType = S[e->to];
            for (i = 0; i != vec_num; ++i) {
                if (S[i] == SetType)
                    S[i] = S[e->from];
            }
        }
        free(e);
    }
    Heap_delete(heap);
    return res;
}

static void w_graph_write_vec(WGraph g, int src, int sink, int weight, void *data) {
    if (src < sink) {//保证一条边只写一次
        fprintf((FILE *) data, "%d -- %d [label=\"%d\"];\n", src, sink, weight);
    }
}

void w_graph_show_dot(WGraph g, char *path) {
    int i, vec_num;
    FILE *file;
    file = fopen(path, "w");
    //起笔
    fprintf(file, "graph prim {\n");
    vec_num = w_graph_vector_count(g);
    for (i = 0; i != vec_num; ++i)
        w_graph_foreach(g, i, w_graph_write_vec, file);
    //封笔
    fprintf(file, "}\n");
    fclose(file);
}

#define BUF_MAX_SIZE (1024)

WGraph w_graph_create_from_file(char *path) {
    
    return NULL;
}
