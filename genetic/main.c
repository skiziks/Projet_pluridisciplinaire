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
}

int scoring(int tab[], int taille) {
    int i, score = 0;
    for(i=0;i<taille-1;++i) {
        score += distance(tab[i], tab[i+1]);
    }
    return score;
}

TabScore* create_tab_score(Point* tab[]) {
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

void print_array(int arr[]) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));

    Point* tab1[N];
    Point* tab2[N];
    int child[N];



    TabScore* parent1 = create_tab_score(tab1);
    TabScore* parent2 = create_tab_score(tab2);
    /*TabScore* childs[1000];*/


    printf("Parent 1: ");
    print_array(parent1->tab);
    printf("Parent 2: ");
    print_array(parent2->tab);

    int i;
    for(i=0;i < 1000;++i) {
        order_crossover(parent1->tab, parent2->tab, child);
        if(scoring(child, N) < 4) {
            printf("Enfant  : ");
            print_array(child);
        }
    }

    return 0;
}
