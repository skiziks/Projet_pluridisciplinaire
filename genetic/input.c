#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "input.h"
#include "matrix.h"

#define MAX_LINE_LENGTH 1024 

/*
int main() {
    
    const char* distance_path = "../livraison85/matrice_distances_m.csv";
    const char* time_path = "../livraison85/matrice_durees_s.csv";

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
    

    char* path = "../livraison85/pharmacies_etudiees.csv"; 
    int n = count_matrix_size(path);
    Point** points = get_points_tab_from_file(path);
    if (points) {
        for (int i = 0; i < n; i++) {
            printf("Point %d: lat = %.6f, lon = %.6f\n", points[i]->id, points[i]->lat, points[i]->lon);
            free(points[i]);
        }
        free(points);
    }

    return 0;
    
}*/


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

double** get_time_seconds_matrix_from_file(char* path) {
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

Point** get_points_tab_from_file(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    int n = count_matrix_size(path);

    Point** points = (Point**)malloc(n * sizeof(Point*));
    if (!points) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    int i = 0;
    while (fgets(line, sizeof(line), file) && i < n) {
        line[strcspn(line, "\r\n")] = 0; // enlève les retours à la ligne

        char* token;
        int field = 0;
        double lon = 0.0;
        double lat = 0.0;

        token = strtok(line, ",");
        while (token != NULL) {
            field++;
            if (field == 3) {
                lon = atof(token);
            } else if (field == 4) {
                lat = atof(token);
            }
            token = strtok(NULL, ",");
        }

        if (field < 4) { // Verifie si on a au moins 4 champs sur la ligne (nom, adresse, lon, lat)
            fprintf(stderr, "Invalid line format at line %d\n", i + 1);
            continue; // skip la ligne invalide
        }

        points[i] = (Point*)malloc(sizeof(Point));
        if (!points[i]) {
            perror("Memory allocation failed");
            for (int j = 0; j < i; j++) free(points[j]);
            free(points);
            fclose(file);
            return NULL;
        }

        points[i]->id = i;
        points[i]->lat = lat;
        points[i]->lon = lon;
        i++;
    }

    fclose(file);
    return points;
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