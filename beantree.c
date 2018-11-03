#include "beantree.h"

beantree *new_beantree(){
    beantree *tree = malloc(sizeof(beantree));
    tree->size = 1;
    tree->n_files = 0;
    // Filenames to null
    tree->files = malloc(sizeof(char*)*tree->size);
    for(int i=0;i<tree->size;i++) tree->files[i] = NULL;
    // Panels to null
    tree->panels = malloc(sizeof(uli*)*(2*tree->size));
    for(int i=0;i<(2*tree->size);i++) tree->panels[i] = NULL;
}

void beantree_update_panels_read_file(beantree *tree, int pos, int only_panel0){
    const char *fname = tree->files[pos];
    FILE *fil = fopen(fname,'r');
    assert(fil!=NULL);
    char buffer[BUFFER_S+1];
    buffer[BUFFER_S] = '\0';
    while(!feof(fil)){
        // Get the next word in the buffer.
        int p = 0;
        while(1){
            char c = fgetc(fil);
            if(c==' ' || feof(fil)) break;
            if(p==BUFFER_S) continue;
            buffer[p++] = c;
        }
        buffer[p] = '\0';
        // Update the panels with the word
        {
            // Get the hashes of the current word:
            uli part1[N_HASHS];
            int part2[N_HASHS];
            for(int j=0;j<N_HASHS;j++){
                uli hash = HASH(buffer,j);
                part1[j] = hash/64;
                part2[j] = hash%64;
            }
            // Update the panels
            int i = only_panel0? 0 : tree->size+tree->n_sets;
            uli p_len = only_panel0? 2*tree->size*PANEL_LEN : PANEL_LEN;
            while(i>=0){
                // Add the 1s to the panel
                for(int j=0;j<N_HASHS;j++){
                    int panel = part1[j]%p_len;
                    tree->panels[i][panel] |= 1<<part2[j];
                }
                // Move up
                i = i==0? -1: i/2;
                p_len *= 2;
            }
        }
    }
    fclose(fil);
}

void beantree_add(beantree *tree, const char *fname){
    // Ensure that the size is enough
    if(tree->n_files == tree->size){
        tree->size *= 2;
        // Realloc filenames
        tree->files = realloc(tree->files,sizeof(char*)*tree->size);
        for(int i=tree->size/2;i<tree->size;i++) tree->files[i] = NULL;
        // Realloc panels
        tree->panels = realloc(tree->panels,sizeof(uli*)*(2*tree->size));
        for(int i=3*tree->size/2;i<(2*tree->size);i++) tree->files[i] = NULL;
        // Lower panels:
        int level_p = 0;
        int level_w = tree->size/2;
        for(int k=tree->size-1;k>=1;k--){
            // Move the panel to its new position.
            tree->panels[k+level_w] = tree->panels[k];
            tree->panels[k] = NULL;
            // Update in-level index
            level_p += 1;
            // Move to the previous level
            if(level_p==level_w){
                level_p = 0;
                level_w /= 2;
            }
        }
        // Move panel 0 to 1
        tree->panels[1] = tree->panels[0];
        tree->panels[0] = NULL;
    }

    // Save the fname:
    tree->files[tree->n_files].name = malloc(sizeof(char)*(strlen(fname)+1));
    strcpy(tree->files[tree->n_files].name,fname);
    tree->n_files += 1;

    // Create missing panels
    {
        int i = tree->size+tree->n_sets;
        uli p_len = PANEL_LEN;
        while(i>=0){
            if(tree->panels[i]==NULL){
                tree->panels[i] = malloc(sizeof(uli)*p_len);
            }else{
                break;
            }
            // Move up
            i = i==0? -1: i/2;
            p_len *= 2;
        }
    }

    // Update the panels
    beantree_update_panels_read_file(tree,tree->n_files-1,0);

    // Demortization, update bits on the first panel.
    int demor_target = tree->n_files-1-tree->size/2;
    beantree_update_panels_read_file(tree,demor_target,1);
}
