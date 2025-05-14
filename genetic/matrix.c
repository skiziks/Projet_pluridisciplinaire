#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"


double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

double** get_distance_meters_matrix_random(int size) {
    return NULL;
}
int** get_time_seconds_matrix_random(int size) {
    return NULL;
}
double** get_distance_meters_matrix_from_file(char* path) {
    return NULL;
}
int** get_time_seconds_matrix_from_file(char* path) {
    return NULL;
}