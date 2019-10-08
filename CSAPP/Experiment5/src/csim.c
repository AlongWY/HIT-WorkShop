#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXLRU (999)

typedef unsigned long ulong;

typedef struct CacheLine {
    int vilad;
    ulong tag;
    int access;
} CacheLine;

typedef struct CacheSet {
    CacheLine *lines;
} CacheSet;

typedef struct Cache {
    int s;
    int S;
    int E;
    int b;
    int B;
    CacheSet *sets;
} Cache;

void printHelp();
void initCache(Cache *pCache);
void freeCache(Cache *pCache);
void updateLru(Cache *pCache, ulong tag, ulong setIndex);
int isHit(Cache *pCache, ulong tag, ulong setIndex);
int updateCache(Cache *pCache, ulong tag, ulong setIndex);
void loadCache(Cache *pCache, int *h, int *m, int *e, ulong addr, int size);
void storeCache(Cache *pCache, int *h, int *m, int *e, ulong addr, int size);
void modifyCache(Cache *pCache, int *h, int *m, int *e, ulong addr, int size);

int verbose;

int main(int argv, char **argc) {
    int hit_count = 0, miss_count = 0, eviction_count = 0;
    /** <s>组索引数 <E>关联度 <b>内存块内地址位数 <t>内存访问轨迹文件名 */
    Cache cache;
    FILE *tracefile = NULL;

    /** 处理输入参数 */
    for (int i = 1; i != argv; ++i) {
        if (0 == strncmp("-s", argc[i], 2)) {
            cache.s = atoi(argc[++i]);
        } else if (0 == strncmp("-E", argc[i], 2)) {
            cache.E = atoi(argc[++i]);
        } else if (0 == strncmp("-b", argc[i], 2)) {
            cache.b = atoi(argc[++i]);
        } else if (0 == strncmp("-t", argc[i], 2)) {
            tracefile = fopen(argc[++i], "re");
        } else if (0 == strncmp("-v", argc[i], 2)) {
            verbose = 1;
        } else if (0 == strncmp("-h", argc[i], 2)) {
            printHelp();
            if (tracefile)
                fclose(tracefile);
            exit(EXIT_SUCCESS);
        }
    }
    initCache(&cache);
    char opt;
    ulong size, addr;
    while (fscanf(tracefile, " %c %lx,%lu", &opt, &addr, &size) == 3) {
        if (verbose)
            printf("%c %lx,%lu ", opt, addr, size);
        switch (opt) {
        case 'L':loadCache(&cache, &hit_count, &miss_count, &eviction_count, addr, size);
            break;
        case 'S':storeCache(&cache, &hit_count, &miss_count, &eviction_count, addr, size);
            break;
        case 'M':modifyCache(&cache, &hit_count, &miss_count, &eviction_count, addr, size);
            break;
        default:break;
        }
        if (verbose)
            printf("\n");
    }

    printSummary(hit_count, miss_count, eviction_count);
    if (tracefile)
        fclose(tracefile);
    freeCache(&cache);
    return EXIT_SUCCESS;
}

void printHelp() {
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("-h         Print this help message.\n");
    printf("-v         Optional verbose flag.\n");
    printf("-s <num>   Number of set index bits.\n");
    printf("-E <num>   Number of lines per set.\n");
    printf("-b <num>   Number of block offset bits.\n");
    printf("-t <file>  Trace file.\n\n\n");
    printf("Examples:\n");
    printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

void initCache(Cache *pCache) {
    pCache->S = 2 << pCache->s;
    pCache->B = 2 << pCache->b;

    pCache->sets = (CacheSet *) malloc(pCache->S * sizeof(CacheSet));

    for (int i = 0; i != pCache->S; ++i) {
        pCache->sets[i].lines = (CacheLine *) malloc(pCache->E * sizeof(CacheLine));
        for (int j = 0; j != pCache->E; ++j) {
            pCache->sets[i].lines[j].vilad = 0;
            pCache->sets[i].lines[j].access = 0;
        }
    }

}

void freeCache(Cache *pCache) {
    for (int i = 0; i != pCache->S; ++i) {
        free(pCache->sets[i].lines);
    }
    free(pCache->sets);
}

void loadCache(Cache *pCache, int *h, int *m, int *e, ulong addr, int size) {
    ulong tag, setIndex;
    addr = addr >> pCache->b;
    setIndex = addr & ((1 << pCache->s) - 1);
    tag = addr >> pCache->s;

    if (isHit(pCache, tag, setIndex)) {
        ++*h;
        if (verbose)
            printf("hit ");
    } else {
        ++*m;
        if (verbose)
            printf("miss ");
        if (updateCache(pCache, tag, setIndex)) {
            ++*e;
            if (verbose)
                printf("eviction ");
        }
    }
}

void storeCache(Cache *pCache, int *h, int *m, int *e, ulong addr, int size) {
    loadCache(pCache, h, m, e, addr, size);
}

void modifyCache(Cache *pCache, int *h, int *m, int *e, ulong addr, int size) {
    loadCache(pCache, h, m, e, addr, size);
    storeCache(pCache, h, m, e, addr, size);
}

int isHit(Cache *pCache, ulong tag, ulong setIndex) {
    for (int i = 0; i != pCache->E; ++i) {
        if (pCache->sets[setIndex].lines[i].vilad && pCache->sets[setIndex].lines[i].tag == tag) {
            for (int j = 0; j != pCache->E; ++j)
                --pCache->sets[setIndex].lines[j].access;
            pCache->sets[setIndex].lines[i].access = MAXLRU;
            return 1;
        }
    }
    return 0;
}

int updateCache(Cache *pCache, ulong tag, ulong setIndex) {
    int i, minAccseeIndex = 0, minAccess = pCache->sets[setIndex].lines[0].access;
    for (i = 0; i != pCache->E; ++i) {
        if (!pCache->sets[setIndex].lines[i].vilad) {
            break;
        } else if (pCache->sets[setIndex].lines[i].access < minAccess) {
            minAccess = pCache->sets[setIndex].lines[i].access;
            minAccseeIndex = i;
        }
    }

    if (i == pCache->E) { //行满
        pCache->sets[setIndex].lines[minAccseeIndex].vilad = 1;
        pCache->sets[setIndex].lines[minAccseeIndex].tag = tag;
        for (int j = 0; j != pCache->E; ++j)
            --pCache->sets[setIndex].lines[j].access;
        pCache->sets[setIndex].lines[minAccseeIndex].access = MAXLRU;
        return 1;
    } else {
        pCache->sets[setIndex].lines[i].vilad = 1;
        pCache->sets[setIndex].lines[i].tag = tag;
        for (int j = 0; j != pCache->E; ++j)
            --pCache->sets[setIndex].lines[j].access;
        pCache->sets[setIndex].lines[i].access = MAXLRU;
        return 0;
    }
}


