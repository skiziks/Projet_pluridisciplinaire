#include "genetic_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int is_start(int id) {
    return id <= 0;
}

void init_random_tab(int tab[]) {
    int i;
    tab[N + NB_TRUCKS_MAX] = 0;
    for(i=0;i<N + NB_TRUCKS_MAX;++i) {
        tab[i] = i - NB_TRUCKS_MAX + 1;
    }
    for(i=1;i<N-1;++i) {
        int j = rand() % (N-2) + 1;
        int temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
    }
}

void permute_tab(int tab[], int nb_permut){
    int n, i, j, temp;
    for(n=0;n<nb_permut;++n) {
        j = rand() % (N-2 + NB_TRUCKS_MAX) + 1; /* %(N-2) + 1 to not exchange the start and finish*/
        i = rand() % (N-2 + NB_TRUCKS_MAX) + 1;
        if (i == j) {
            j = (j+1) % (N-2) + 1;
        }
        temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
    }
}

void move_in_tab(int tab[]) {
    int i = rand() % (N-1 + NB_TRUCKS_MAX) +1;
    int j = rand() % (N-1 + NB_TRUCKS_MAX) +1;
    if (i == j) return;

    int city = tab[i];

    if (i < j) {
        memmove(&tab[i], &tab[i+1], (j - i) * sizeof(int)); /*memmove: Astuce du C pour déplacer une partie d'un tableau, ça reste O(n)*/
        tab[j] = city;
    } else {
        memmove(&tab[j+1], &tab[j], (i - j) * sizeof(int));
        tab[j] = city;
    }
}

void reverse_segment(int tab[], int i, int j) {
    while (i < j) {
        int temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
        i++;
        j--;
    }
}

/*Effectue une mutation en inversant un segment*/
void two_opt(int tab[]) {
    int i = rand() % (N-1 + NB_TRUCKS_MAX) + 1;
    int j = rand() % (N-1 + NB_TRUCKS_MAX) + 1;
    if (i == j) return;

    if (i > j) {
        reverse_segment(tab, j, i);
    } else {
        reverse_segment(tab, i, j);
    }
}

void mutate(int tab[]) {
    int r = rand() % 100;

    if (r < 50) {
        permute_tab(tab, r % 5 + 1);
    } else if (r < 75) {
        move_in_tab(tab);
    } else {
        two_opt(tab);
    }
}


double scoring(int tab[], double matrix[][N]) {
    int i;
    double score = 0;
    int camion = 0;
    for(i=0;i<N + NB_TRUCKS_MAX;++i) {
        
        camion++;
        if (is_start(tab[i])) {
            if(camion > N/NB_TRUCKS_MAX*1.5) {
                score += 1000000;
            }
            camion = 0;
        }
        int dist = distance(tab[i], tab[i+1], matrix)*1000;
        score += dist/* < 0.1 ? 1000000 : dist forcing exact number of trucks*/;
    }
    if(camion > N/NB_TRUCKS_MAX*2) {
                score += 1000000;
            }
    

    return score;
}

TabScore* create_tab_score_from_int(int tab[], double matrix[][N]) {
    TabScore* ts = malloc(sizeof(TabScore));
    int i;
    for(i = 0; i < N + NB_TRUCKS_MAX +1; i++) {
        ts->tab[i] = tab[i];
    }
    ts->score = scoring(ts->tab, matrix);
    return ts;
}

TabScore* create_tab_score_from_points(Point* tab[], double matrix[][N]) {
    TabScore* ts = malloc(sizeof(TabScore));
    int i;
    for(i = 0; i < N + NB_TRUCKS_MAX + 1; i++) {
        ts->tab[i] = tab[i]->id;
    }
    ts->score = scoring(ts->tab, matrix);
    return ts;
}

void free_tab_score(TabScore* ts) {
    free(ts);
}


void print_array_simple(int arr[]) {
    int i;
    for (i = 0; i < N + NB_TRUCKS_MAX +1; i++) {
        printf("%d ", arr[i]);
    }
    putchar('\n');
}

void order_crossover(int parent1[], int parent2[], int child[]) {
    int start, end, i, j, k;
    int used[N + NB_TRUCKS_MAX] = {0};


    child[0] = 0;
    child[N + NB_TRUCKS_MAX] = 0;
    /*used[0] = 1;*/
    start = rand() % (N-2) + 1;
    end = rand() % (N-2) + 1;
    if (start > end) { int temp = start; start = end; end = temp; }
    for (i = start; i <= end; i++) {
        child[i] = parent1[i];
        used[parent1[i]+NB_TRUCKS_MAX-1] = 1;
    }

    j = 1; 
    k = 1;  
    while (k < N + NB_TRUCKS_MAX && j < N + NB_TRUCKS_MAX) {
        
        if (k >= start && k <= end) {
            k = end + 1; 
            continue;
        }
        if (!used[parent2[j]+NB_TRUCKS_MAX-1]) {
            child[k] = parent2[j];
            used[parent2[j]+NB_TRUCKS_MAX-1] = 1;
            k++;
        }
        j++;
    }
    /*Mutate the child*/
    mutate(child);
}

void quick_sort_children(TabScore* tab[], int start, int end) {
    if (end <= start) return;
    int pivot = tab[end]->score;
    int i = start - 1;
    int j;
    for (j = start; j < end; j++) {
        if (tab[j]->score < pivot) {
            i++;
            TabScore* temp = tab[i];
            tab[i] = tab[j];
            tab[j] = temp;
        }
    }
    TabScore* temp = tab[i+1];
    tab[i+1] = tab[end];
    tab[end] = temp;
    quick_sort_children(tab, start, i);
    quick_sort_children(tab, i+2, end);
}

void init_children(TabScore* tab[], int size, Point* tab_points[], double matrix[][N]) {
    int i;
    for (i = 0; i < size; i++) {
        tab[i] = create_tab_score_from_points(tab_points, matrix);
        init_random_tab(tab[i]->tab);
        tab[i]->score = scoring(tab[i]->tab, matrix);
    }
}

void print_array(int arr[], double matrix[][N]) {
    int i;
    for (i = 0; i < N + NB_TRUCKS_MAX; i++) {
        printf("%d ", arr[i]);
    }
    printf("Score: %f", scoring(arr, matrix));
    printf("\n");
}

void get_2_random_parents(TabScore* parents[], TabScore** parent1, TabScore** parent2, int max, double matrix[][N]) {
    int i, j;


    double r1 = ((double)rand() / RAND_MAX) * max;
    double r2 = ((double)rand() / RAND_MAX) * max;
    i = (int)r1;
    j = (int)r2;

    *parent1 = parents[i];
    *parent2 = parents[j];

    if (*parent1 == *parent2) {
        *parent2 = parents[(j + 1) % max];
    }
}

void copy_children_to_parents(TabScore* parents[], TabScore* children[], int max) {
    int i;
    for (i = 0; i < max; i++) {
        free_tab_score(parents[i]);
        parents[i] = children[i];
        children[i] = NULL;
    }
}

int count_same_first_score(TabScore* parents[], int max) {
    int i;
    int count = 0;
    for (i = 0; i < max && parents[i]->score == parents[0]->score; i++) {
        count++;
    }
    return count;
}