#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"

double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

void print_points_from_array(int arr[], Point* points[]) {
    int i;
    for (i = 0; i < N; i++) {
        printf("(%f, %f) ", points[arr[i]]->lat, points[arr[i]]->lon);
    }
    printf("\n");
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

double calc_dist(Point* first, Point* second) {
    return sqrt((first->lat - second->lat) * (first->lat - second->lat) + (first->lon - second->lon) * (first->lon - second->lon));
}

void fill_matrix(Point* tab[], double matrix[][N]) {
    int i, j;
    for(i=0;i<N;++i) {
        for(j=0;j<N;++j) {
            matrix[i][j] = calc_dist(tab[i], tab[j]);
        }
    }
}

double distance(int first, int second, double matrix[][N]) {
    return matrix[first][second];
}