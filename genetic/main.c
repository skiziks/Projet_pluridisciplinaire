#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 10 

typedef struct _point {
    int id;
    double lat;
    double lon;
} Point;

/*Struct qui représente un tableau et son scoring*/
typedef struct _tabscore {
    int tab[N];
    int score;
} TabScore;

double matrix[N][N];

double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

void permutTab(int tab[]){
    int i, j, temp;
    for(i=1;i<N-1;++i) {
        j = rand() % (N-2) + 1;
        temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
    }
}


double calculateDistance(Point* first, Point* second) {
    return sqrt((first->lat - second->lat) * (first->lat - second->lat) + (first->lon - second->lon) * (first->lon - second->lon));
}

void fillMatrix(Point* tab[]) {
    int i, j;
    for(i=0;i<N;++i) {
        for(j=0;j<N;++j) {
            matrix[i][j] = calculateDistance(tab[i], tab[j]);
        }
    }
}

double distance(int first, int second) {
    return matrix[first][second];
}

void allocAndFillTab(Point* tab[]) {
    int i;
    for(i=0;i<N;++i) {
        tab[i] = malloc(sizeof(Point));
        tab[i]->id = i;
        tab[i]->lat = random_double(48.3, 49.2);
        tab[i]->lon = random_double(2.8, .9);
    }
    tab[N-1] = tab[0];
}

int scoring(int tab[], int taille) {
    int i, score = 0;
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

void print_array(int arr[]) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }
    printf("Score: %d", scoring(arr, N));
    printf("\n");
}

int main() {
    srand(time(NULL));

    Point* tab1[N];
    Point* tab2[N];
    int child[N];

    allocAndFillTab(tab1);
    allocAndFillTab(tab2);
    fillMatrix(tab1);


    TabScore* parent1 = create_tab_score_from_points(tab1);
    TabScore* parent2 = create_tab_score_from_points(tab2);
    TabScore* childs[1000];

    permutTab(parent1->tab);
    permutTab(parent2->tab);
    parent1->score = scoring(parent1->tab, N);
    parent2->score = scoring(parent2->tab, N);


    printf("Parent 1: ");
    print_array(parent1->tab);
    printf("Parent 2: ");
    print_array(parent2->tab);

    int minScoreParent = parent1->score < parent2->score ? parent1->score : parent2->score;

    int nb_meilleur = 0;

    int i;
    for(i=0;i < 1000;++i) {
        order_crossover(parent1->tab, parent2->tab, child);
        if(scoring(child, N) < minScoreParent) {
            printf("Enfant  : ");
            print_array(child);
            nb_meilleur++;
        }
        childs[i] = create_tab_score_from_int(child);
    }

    quick_sort_children(childs, 0, 999);
    printf("100 premiers enfants triés : \n");
    for(i=0;i<100;++i) {
        print_array(childs[i]->tab);
    }

    printf("Nombre de meilleur enfant : %d sur 1000\n", nb_meilleur);

    return 0;
}
