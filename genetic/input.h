#ifndef _input_

#include "matrix.h"

// Recupere une matrice de distance à partir d'un fichier
double** get_distance_meters_matrix_from_file(char* path);

// Recupere une matrice d'entiers à partir d'un fichier
double** get_time_seconds_matrix_from_file(char* path);

// Recupere les coordonnées des pharmacies à partir d'un fichier, puis crée des points avec 
Point** get_points_tab_from_file(char* path);

// Affiche une matrice de doubles
void print_double_matrix(double** matrix, int n);

// Affiche une matrice d'entiers
void print_int_matrix(int** matrix, int n);

// Libère la mémoire d'une matrice de doubles
void free_double_matrix(double** matrix, int n);

// Libère la mémoire d'une matrice d'entiers
void free_int_matrix(int** matrix, int n);

// Compte la taille de la matrice en fonction du nombre de lignes
int count_matrix_size(const char* path);

#endif