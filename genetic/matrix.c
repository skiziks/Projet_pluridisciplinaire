#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"

#define MAX_LINE_LENGTH 1024 
/*
int main() {
    const char* distance_path = "../livraison10/matrice_distances_m.csv";
    const char* time_path = "../livraison10/matrice_durees_s.csv";

    int n1 = count_matrix_size(distance_path);
    int n2 = count_matrix_size(time_path);
    printf("n1 = %d, n2 = %d\n", n1, n2);

    if (n1 <= 0 || n2 <= 0 || n1 != n2) {
        fprintf(stderr, "Erreur : matrices non carrées ou tailles différentes.\n");
        return 1;
    }

    double** distances = get_distance_meters_matrix_from_file((char*)distance_path);
    int** times = get_time_seconds_matrix_from_file((char*)time_path);

    if (!distances || !times) {
        fprintf(stderr, "Erreur lors de la lecture des fichiers.\n");
        return 1;
    }

    printf("=== Matrice des distances (mètres) ===\n");
    print_double_matrix(distances, n1);

    printf("\n=== Matrice des temps (secondes) ===\n");
    print_int_matrix(times, n2);

    free_double_matrix(distances, n1);
    free_int_matrix(times, n2);

    return 0;
}
*/
double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

double** get_distance_meters_matrix_from_file(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    int n = count_matrix_size(path);

    double** matrix = malloc(n * sizeof(double*));
    if (!matrix) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(double));
        if (!matrix[i]) {
            perror("Memory allocation failed");
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            fclose(file);
            return NULL;
        }

        for (int j = 0; j < n; j++) {
            if (fscanf(file, "%lf", &matrix[i][j]) != 1) {
                perror("Invalid matrix data");
                for (int k = 0; k <= i; k++) free(matrix[k]);
                free(matrix);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}

int** get_time_seconds_matrix_from_file(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    int n = count_matrix_size(path);

    int** matrix = malloc(n * sizeof(int*));
    if (!matrix) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(int));
        if (!matrix[i]) {
            perror("Memory allocation failed");
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            fclose(file);
            return NULL;
        }

        for (int j = 0; j < n; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                perror("Invalid matrix data");
                for (int k = 0; k <= i; k++) free(matrix[k]);
                free(matrix);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}

int count_matrix_size(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return -1;

    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }

    fclose(file);
    return count;
}


void free_double_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void free_int_matrix(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
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

void print_double_matrix(double** matrix, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%8.2f ", matrix[i][j]); // 8.2f = affichage formaté pour que ça s'affiche joli 
        }
        putchar('\n');
    }
}

void print_int_matrix(int** matrix, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%4d ", matrix[i][j]); // 4d = affichage formaté pour que ça s'affiche joli 
        }
        putchar('\n');
    }
}


double distance(int first, int second, double matrix[][N]) {
    return matrix[first][second];
}