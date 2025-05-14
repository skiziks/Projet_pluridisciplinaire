#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <MLV/MLV_all.h>
#include "interface.h"

#define NB_INDIVIDU 20000

#define PERCENT_KEEP 5
#define IND_KEEP (NB_INDIVIDU*PERCENT_KEEP/100)
#define PERCENT_MUTATE 25
#define IND_MUTATE (NB_INDIVIDU*PERCENT_MUTATE/100 + IND_KEEP)
#define PERCENT_CROSSOVER 40
#define IND_CROSSOVER (NB_INDIVIDU*PERCENT_CROSSOVER/100 + IND_MUTATE)
#define PERCENT_NEW 30
#define IND_NEW (NB_INDIVIDU*PERCENT_NEW/100 + IND_CROSSOVER)


#define PERCENT_REPRODUCTION 30


double matrix[N][N];

double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

void init_random_tab(int tab[]) {
    int i;
    tab[N-1] = 0;
    tab[N-2] = 0;
    for(i=0;i<N-2;++i) {
        tab[i] = i;
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
        j = rand() % (N-2) + 1; /* %(N-2) + 1 to not exchange the start and finish*/
        i = rand() % (N-2) + 1;
        if (i == j) {
            j = (j+1) % (N-2) + 1;
        }
        temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
    }
}

void move_in_tab(int tab[]) {
    int i = rand() % (N-2) +1;
    int j = rand() % (N-2) +1;
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
    int i = rand() % (N-2) + 1;
    int j = rand() % (N-2) + 1;
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
    int camion;
    for(i=0;i<taille-1;++i) {
        if (tab[i] == 0 && i != 0 && i != taille-1) {
            camion = i;
        }
        score += distance(tab[i], tab[i+1])*1000;
    }

    if (camion < N/4 || camion > 3*N/4) {
        score += 50000*(N/2 - abs(camion - N/2));
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
    int used[N-1] = {0};

    child[0] = 0;
    child[N-1] = 0;
    /*used[0] = 1;*/

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

void init_children(TabScore* tab[], int size, Point* tab_points[]) {
    int i;
    for (i = 0; i < size; i++) {
        tab[i] = create_tab_score_from_points(tab_points);
        init_random_tab(tab[i]->tab);
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


    double r1 = ((double)rand() / RAND_MAX) * max * PERCENT_REPRODUCTION / 100;
    double r2 = ((double)rand() / RAND_MAX) * max * PERCENT_REPRODUCTION / 100;
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


int main() {
    srand(time(NULL));

    create_windows();

    Point* tab1[N];
    int child[N];
    MLV_Color colors[2];

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

    for(s=0;s<nb_gen;++s) {

        int i;
        TabScore* parent1;
        TabScore* parent2;

        /*Keep*/
        for(i=0;i < IND_KEEP;++i) {
            childs[i] = create_tab_score_from_int(parents[i]->tab);
        }
        /*Mutate*/
        for(;i < IND_MUTATE;++i) {
            childs[i] = create_tab_score_from_int(parents[i%(PERCENT_REPRODUCTION * N / 100)]->tab);
            mutate(childs[i]->tab);
            childs[i]->score = scoring(childs[i]->tab, N);
        }
        /*Crossover*/
        for(;i < IND_CROSSOVER;++i) {
            get_2_random_parents(parents, &parent1, &parent2, 300);
            order_crossover(parent1->tab, parent2->tab, child);
            childs[i] = create_tab_score_from_int(child);
        }
        /*New*/
        for(;i < IND_NEW;++i) {
            childs[i] = create_tab_score_from_points(tab1);
            init_random_tab(childs[i]->tab);
            childs[i]->score = scoring(childs[i]->tab, N);
        }

        quick_sort_children(childs, 0, NB_INDIVIDU-1);
        /*printf("100 premiers enfants triés : \n");
        for(i=0;i<100;++i) {
            print_array(childs[i]->tab);
        }*/
        copy_children_to_parents(parents, childs, NB_INDIVIDU);
        clear_window();
        for(i=255;i>=0;i -= 5) {
            colors[0] = MLV_rgba(255-i, 128-(i/2), 128-(i/2), 255);
            colors[1] = MLV_rgba(128-(i/2), 128-(i/2), 255-i, 255);
            show_path(tab1, parents[i]->tab, colors, 2, i==0);
        }
        actualise_window();
        printf("Generation %d : Best score: %f Median: %f\n", s , parents[0]->score, parents[NB_INDIVIDU/2]->score);
        
        printf("Number of same score: %d\n", count_same_first_score(parents, NB_INDIVIDU));
    }
    printf("Best score: %f\n", parents[0]->score);
    printf("Median : %f\n", parents[NB_INDIVIDU/2]->score);
    print_points_from_array(parents[0]->tab, tab1);

    print_matrix();
    actualise_window();
    clear_window();
    actualise_window();
    colors[0] = MLV_rgba(255, 128, 128, 255);
    colors[1] = MLV_rgba(128, 128, 255, 255);
    show_path(tab1, parents[0]->tab, colors, 2, 1);
    actualise_window();

    pause_keyboard();
    pause_keyboard();
    return 0;
}
