#ifndef BEANTREE_H
#define BEANTREE_H

#include <stdlib.h>
#include <string.h>


#define PANEL_LEN 4
// Max word size:
#define BUFFER_S 32

typedef unsigned long long int uli;

typedef struct {
    int size;
    int n_files;
    char **files;
    uli **panels;
    // panels[0] is the panel that will be used after a size increase (demortization)
} beantree;

beantree *new_beantree();
void beantree_add(beantree *tree, const char *fname);
int beantree_locate(const beantree *tree, const strset *terms,
    const strset *found, int max_found);
void beantree_free(beantree *tree);

#endif
