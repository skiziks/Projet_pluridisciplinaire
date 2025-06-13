#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"


/**
 * * @brief Retourne 0 si value < 0, sinon retourne value
 */
int z(int value) {
    return value < 0 ? 0 : value;
}
double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

void print_points_from_array(int arr[], Point* points[], int N) {
    int i;
    for (i = 0; i < N; i++) {
        printf("(%f, %f) ", points[z(arr[i])]->lat, points[z(arr[i])]->lon);
    }
    printf("\n");
}


void alloc_and_fill_tab(Point* tab[], int N) {
    int i;
    for(i=0;i<N;++i) {
        tab[i] = malloc(sizeof(Point));
        tab[i]->id = i;
        tab[i]->lat = random_double(0, 50);
        tab[i]->lon = random_double(0, 50);
    }
    
    for(i=N;i<N+NB_TRUCKS_MAX+1;++i) {
        tab[i] = tab[0];
    }
}

Point** copy_with_trucks(Point* tab[], int N) {
    Point** tab2 = malloc(sizeof(Point*) * (N + NB_TRUCKS_MAX + 1));

    int i;
    for(i=0;i<N+1;++i) {
        tab2[i] = malloc(sizeof(Point));
        tab2[i]->id = tab[i]->id;
        tab2[i]->lat = tab[i]->lat;
        tab2[i]->lon = tab[i]->lon;
    }
    
    for(i=N+1;i<N+NB_TRUCKS_MAX+1;++i) {
        tab2[i] = tab2[0];
    }

    return tab2;
}

double calc_dist(Point* first, Point* second) {
    return sqrt((first->lat - second->lat) * (first->lat - second->lat) + (first->lon - second->lon) * (first->lon - second->lon));
}

void fill_matrix(Point* tab[], double **matrix, int N) {
    int i, j;
    for(i=0;i<N;++i) {
        for(j=0;j<N;++j) {
            matrix[i][j] = calc_dist(tab[i], tab[j]);
        }
    }
}

double distance(int first, int second, double **matrix) {
    return matrix[z(first)][z(second)];
}