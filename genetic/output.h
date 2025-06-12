#ifndef OUTPUT_H
#define OUTPUT_H

#include "matrix.h"
#include "genetic_utils.h"

/**
 * @brief Écrit les chemins dans un fichier, une ligne par camion
 * @param ts Le tableau de score à écrire
 * @param dist_matrix La matrice de distances
 * @param time_matrix La matrice de temps
 * @param N Le nombre de points (sans les camions)
 * @param path Le chemin du fichier de sortie
 */
void write_output_to_file(TabScore *ts, double **dist_matrix, double **time_matrix, int N, const char *path);

#endif
