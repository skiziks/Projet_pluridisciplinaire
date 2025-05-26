#ifndef _MATRIX_
#define _MATRIX_
#include <math.h>

#define N 85 /* Number of points*/

#define NB_TRUCKS_MAX 10

typedef struct _point {
    int id;
    double lat;
    double lon;
} Point;

/**
 * @brief Calcule la distance entre 2 points
 * @param first Point1
 * @param second Point2
 * @return distance entre les 2 points
 */
double calc_dist(Point* first, Point* second);

/**
 * @brief Effectue l'allocation et remplie un tableau de points aléatoires
 * @param tab
 */
void alloc_and_fill_tab(Point* tab[]);

/**
 * @brief Affiche les coordonnées correspondantes au tableau
 */
void print_points_from_array(int arr[], Point* points[]);

/**
 * @brief Remplit la matrice de distance en calculant toutes les distance de paires de points
 * @param tab tableau de points
 * @param matrix matrice de distance à remplir
 */
void fill_matrix(Point* tab[], double matrix[][N]);
/**
 * @brief Affiche la matrice de distance
 */
void print_matrix(double matrix[][N]);

/**
 * @brief Récupère la distance entre 2 points à partir de la matrice de distance
 * @param first id du premier point
 * @param second id du deuxième point
 * @param matrix matrice de distance
 */
double distance(int first, int second, double matrix[][N]);

/**
 * * @brief Retourne 0 si value < 0, sinon retourne value
 */
int z(int value);

double** get_distance_meters_matrix_from_file(char* path);
int** get_time_seconds_matrix_from_file(char* path);


#endif