//
// Created by along on 17-9-17.
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void *mcalloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0)
        return NULL;

    void *memory = malloc(nmemb * size);
    memset(memory, 0, nmemb * size);
    return memory;
}

int main() {
    int *mem = mcalloc(0, sizeof(int));
    printf("%d", mem == NULL);
    if (mem != NULL)
        free(mem);
    return 0;
}

