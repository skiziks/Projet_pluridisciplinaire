#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "interface.h"

#define PERCENT_KEEP 5
#define PERCENT_MUTATE 20
#define PERCENT_CROSSOVER 25
#define PERCENT_NEW 50

#define NB_INDIVIDU 10000


double matrix[N][N];

double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

void permute_tab(int tab[], int nb_permut){
    int n, i, j, temp;
    for(n=0;n<nb_permut;++n) {
        j = rand() % (N-2) + 1;
        i = rand() % (N-2) + 1;
        if (i == j) {
            j = (j+1) % (N-2) + 1;
        }
        temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
    }
}


double calc_dist(Point* first, Point* second) {
    return sqrt((first->lat - second->lat) * (first->lat - second->lat) + (first->lon - second->lon) * (first->lon - second->lon));
}

void fill_matrix(Point* tab[]) {
    int i, j;
    for(i=0;i<N;++i) {
        for(j=0;j<N;++j) {
            matrix[i][j] = calc_dist(tab[i], tab[j]);
        }
    }
}

void print_matrix() {
    int i, j;
    for(i=0;i<N;++i) {
        for(j=0;j<N;++j) {
            printf("%f ", matrix[i][j]);
        }
        putchar('\n');
    }
}

double distance(int first, int second) {
    return matrix[first][second];
}

void alloc_and_fill_tab(Point* tab[]) {
    int i;
    for(i=0;i<N;++i) {
        tab[i] = malloc(sizeof(Point));
        tab[i]->id = i;
        tab[i]->lat = random_double(0, 50);
        tab[i]->lon = random_double(0, 50);
    }
    tab[N-1] = tab[0];
}

double scoring(int tab[], int taille) {
    int i;
    double score = 0;
    for(i=0;i<taille-1;++i) {
        score += distance(tab[i], tab[i+1])*1000;
    }
    return score;
}

TabScore* create_tab_score_from_int(int tab[]) {
    TabScore* ts = malloc(sizeof(TabScore));
    int i;
    for(i = 0; i < N; i++) {
        ts->tab[i] = tab[i];
    }
    ts->score = scoring(ts->tab, N);
    return ts;
}

TabScore* create_tab_score_from_points(Point* tab[]) {
    TabScore* ts = malloc(sizeof(TabScore));
    int i;
    for(i = 0; i < N; i++) {
        ts->tab[i] = tab[i]->id;
    }
    ts->score = scoring(ts->tab, N);
    return ts;
}

void free_tab_score(TabScore* ts) {
    free(ts);
}

void order_crossover(int parent1[], int parent2[], int child[]) {
    int start, end, i, j, k;
    int used[N] = {0};

    child[0] = 0;
    child[N-1] = 0;
    used[0] = 1;

    start = rand() % (N-2) + 1;
    end = rand() % (N-2) + 1;
    if (start > end) { int temp = start; start = end; end = temp; }

    for (i = start; i <= end; i++) {
        child[i] = parent1[i];
        used[parent1[i]] = 1;
    }

    j = 1; 
    k = 1;  
    while (k < N-1) {
        if (k >= start && k <= end) {
            k = end + 1; 
            continue;
        }
        if (!used[parent2[j]]) {
            child[k] = parent2[j];
            used[parent2[j]] = 1;
            k++;
        }
        j++;
    }
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

void init_children(TabScore* tab[], int size, Point* tab_points[]) {
    int i;
    for (i = 0; i < size; i++) {
        tab[i] = create_tab_score_from_points(tab_points);
        permute_tab(tab[i]->tab, N*80);
        tab[i]->score = scoring(tab[i]->tab, N);
    }
}

void print_array(int arr[]) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }
    printf("Score: %f", scoring(arr, N));
    printf("\n");
}

void print_points_from_array(int arr[], Point* tab[]) {
    int i;
    for (i = 0; i < N; i++) {
        printf("(%f, %f) ", tab[arr[i]]->lat, tab[arr[i]]->lon);
    }
    printf("\n");
}

void get_2_random_parents(TabScore* parents[], TabScore** parent1, TabScore** parent2, int max) {
    int i, j;
    double total_score = 0;
    double cumulative[max];
    
    for (i = 0; i < max; i++) {
        total_score += 1.0 / parents[i]->score;
        cumulative[i] = total_score;
    }


    double r1 = ((double)rand() / RAND_MAX) * total_score;
    double r2 = ((double)rand() / RAND_MAX) * total_score;


    for (i = 0; i < max; i++) {
        if (r1 <= cumulative[i]) {
            *parent1 = parents[i];
            break;
        }
    }
    for (j = 0; j < max; j++) {
        if (r2 <= cumulative[j]) {
            *parent2 = parents[j];
            break;
        }
    }

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


int main() {
    srand(time(NULL));

    create_windows();

    Point* tab1[N];
    int child[N];

    alloc_and_fill_tab(tab1);
    fill_matrix(tab1);

    TabScore* parents[NB_INDIVIDU];
    init_children(parents, NB_INDIVIDU, tab1);
    quick_sort_children(parents, 0, NB_INDIVIDU-1);
    TabScore* childs[NB_INDIVIDU];


    int s;
    int nb_gen = 1000;
    print_points_from_array(parents[0]->tab, tab1);
    clear_window();
    show_path(tab1, parents[0]->tab);

    for(s=0;s<nb_gen;++s) {

        int i;
        TabScore* parent1;
        TabScore* parent2;

        for(i=0;i < NB_INDIVIDU*PERCENT_KEEP/100;++i) {
            childs[i] = create_tab_score_from_int(parents[i]->tab);
        }
        for(;i < NB_INDIVIDU*PERCENT_KEEP/100 + NB_INDIVIDU*PERCENT_MUTATE/100;++i) {
            childs[i] = create_tab_score_from_int(parents[i]->tab);
            permute_tab(childs[i]->tab, N/10);
            childs[i]->score = scoring(childs[i]->tab, N);
        }
        for(;i < NB_INDIVIDU*PERCENT_KEEP/100 + NB_INDIVIDU*PERCENT_MUTATE/100 + NB_INDIVIDU*PERCENT_CROSSOVER/100;++i) {
            get_2_random_parents(parents, &parent1, &parent2, 300);
            order_crossover(parent1->tab, parent2->tab, child);
            childs[i] = create_tab_score_from_int(child);
        }
        for(;i < NB_INDIVIDU*PERCENT_KEEP/100 + NB_INDIVIDU*PERCENT_MUTATE/100 + NB_INDIVIDU*PERCENT_CROSSOVER/100 + NB_INDIVIDU*PERCENT_NEW/100;++i) {
            childs[i] = create_tab_score_from_points(tab1);
            permute_tab(childs[i]->tab, N*10);
            childs[i]->score = scoring(childs[i]->tab, N);
        }

        quick_sort_children(childs, 0, NB_INDIVIDU-1);
        /*printf("100 premiers enfants triés : \n");
        for(i=0;i<100;++i) {
            print_array(childs[i]->tab);
        }*/
        copy_children_to_parents(parents, childs, NB_INDIVIDU);
        clear_window();
        show_path(tab1, parents[0]->tab);
        printf("Generation %d : Best score: %f\n", s , parents[0]->score);
    }
    printf("Best score: %f\n", parents[0]->score);
    print_points_from_array(parents[0]->tab, tab1);

    print_matrix();
    return 0;
}
