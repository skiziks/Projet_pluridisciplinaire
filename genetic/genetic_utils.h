#ifndef _GENETIC_UTILS_
#define _GENETIC_UTILS_

#include "matrix.h"


/*Struct qui représente un tableau et son scoring*/
typedef struct _tabscore {
    int *tab;
    double score;
} TabScore;

void print_array_simple(int arr[], int N);

/**
 * * @brief Crée un tableau de score à partir d'un tableau des id des points de taille N
 * @param tab tableau de points de taille N + NB_TRUCKS_MAX
 * @param matrix matrice de distance entre les points
 */
TabScore* create_tab_score_from_int(int tab[], double** matrix, int N);


/**
 * @brief Crée un tableau de score à partir d'un tableau de points de taille N
 * @param tab_points tableau de points de taille N + NB_TRUCKS_MAX
 * @param matrix matrice de distance entre les points
 */
TabScore* create_tab_score_from_points(Point* tab_points[], double** matrix, int N);

/**
 * @brief free la mémoire d'un tableau de score
 */
void free_tab_score(TabScore* ts);

/**
 * @brief Initialise des tableaux de score à partir de permutations aléatoires des points de taille N
 * @param tab tableau de score de taille nb_individu
 * @param nb_individu nombre d'individus
 * @param tab_points tableau de points de taille N + NB_TRUCKS_MAX
 * @param matrix matrice de distance entre les points
 */
void init_children(TabScore* tab[], int nb_individu, Point* tab_points[], double **matrix, int N);

/**
 * @brief Initialise le tableau de taille N avec une permutation aléatoire d'une route
 * @param tab tableau de taille N + NB_TRUCKS_MAX
 */
void init_random_tab(int tab[], int N);

/**
 * @brief Effectue une de ces 3 mutations aléatoirement: Random swap (50%), décalage (20%), random segment inversion (20%)
 * @param tab tableau de taille N + NB_TRUCKS_MAX
 */
void mutate(int tab[], int N);

/**
 * @brief Choisi aléatoirement 2 parents parmi les `max` premiers individus
 * @param parents tableau de score de taille max ou plus (NB_INDIVIDU)
 * @param[out] parent1 pointeur vers le premier parent
 * @param[out] parent2 pointeur vers le deuxième parent
 * @param max
 * @param matrix matrice de distance entre les points
 */
void get_2_random_parents(TabScore* tab[], TabScore** parent1, TabScore** parent2, int max, double **matrix, int N);

/**
 * @brief Effectue un tri rapide entre les individus en fonction de leur score pour avoir les meilleurs en premier
 * @param tab tableau de score de taille nb_individu
 * @param start index de début
 * @param end index de fin
 */
void quick_sort_children(TabScore* tab[], int start, int end);

/**
 * @brief Calcule le score (fitness) d'un individu
 * @param tab tableau de taille N + NB_TRUCKS_MAX
 * @param matrix matrice de distance entre les points
 * @return score de l'individu
 */
double scoring(int tab[], double **matrix, int N);

/**
 * @brief Affiche un tableau
 */
void print_array(int arr[], double **matrix, int N);


/**
 * @brief Compte le nombre d'individus ayant le même score que le premier
 * @param parents tableau de score de taille max ou plus (NB_INDIVIDU)
 * @param max taille du tableau
 * @return nombre d'individus ayant le même score que le premier
 */
int count_same_first_score(TabScore* parents[], int max);

/**
 * @brief 
 */
void copy_children_to_parents(TabScore* parents[], TabScore* children[], int max);

/**
 * @brief Effectue un crossover entre deux parents pour créer un enfant
 * @param parent1 tableau de taille N
 * @param parent2 tableau de taille N
 * @param[out] child tableau de taille N de l'enfant créé
 */
void order_crossover(int parent1[], int parent2[], int child[], int N);


#endif