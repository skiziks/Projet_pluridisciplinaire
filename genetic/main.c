#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <MLV/MLV_all.h>
#include "interface.h"
#include "matrix.h"
#include "input.h"

#define NB_INDIVIDU 10000

#define PERCENT_REPRODUCTION 0.1

#define PERCENT_KEEP 0.1
#define IND_KEEP (NB_INDIVIDU*PERCENT_KEEP/100)
#define PERCENT_MUTATE 40
#define IND_MUTATE (NB_INDIVIDU*PERCENT_MUTATE/100 + IND_KEEP)
#define PERCENT_CROSSOVER 39.9
#define IND_CROSSOVER (NB_INDIVIDU*PERCENT_CROSSOVER/100 + IND_MUTATE)
#define PERCENT_NEW 20
#define IND_NEW (NB_INDIVIDU*PERCENT_NEW/100 + IND_CROSSOVER)


void fill_color_10(MLV_Color colors[], int i) {
    colors[0] = MLV_rgba(255-i, 0, 0, 255);
    colors[1] = MLV_rgba(0, 0, 255-i, 255);
    colors[2] = MLV_rgba(0, 255-i, 0, 255);
    colors[3] = MLV_rgba(255-i, 255-i, 0, 255);
    colors[4] = MLV_rgba(255-i, 0, 255-i, 255);
    colors[5] = MLV_rgba(0, 255-i, 255-i, 255);
    colors[6] = MLV_rgba(255-i, 255-i, 255-i, 255);
    colors[7] = MLV_rgba(255-i, 128-i/2, 128, 255);
    colors[8] = MLV_rgba(128-i/2, 255-i, 70, 255);
    colors[9] = MLV_rgba(0, 255-i, 128-i/2, 255);
}




int main() {
    srand(time(NULL));

    create_windows();

    int N = count_matrix_size("../livraison85/matrice_durees_s.csv") - 1;

    double **matrix = get_time_seconds_matrix_from_file("../livraison85/matrice_durees_s.csv");
    Point** points = get_points_tab_from_file("../livraison85/pharmacies_etudiees.csv");

    Point** tab1 = copy_with_trucks(points, N);
    free(points);
    int *child = malloc((N + NB_TRUCKS_MAX + 1) * sizeof(int));
    MLV_Color colors[10];
    TabScore* parents[NB_INDIVIDU];
    init_children(parents, NB_INDIVIDU, tab1, matrix, N);
    quick_sort_children(parents, 0, NB_INDIVIDU-1);
    TabScore* childs[NB_INDIVIDU];


    int s;
    int nb_gen = 1000;
    print_points_from_array(parents[0]->tab, tab1, N);
    clear_window();

    for(s=0;s<nb_gen;++s) {

        int i;
        TabScore* parent1;
        TabScore* parent2;

        /*Keep*/
        for(i=0;i < IND_KEEP;++i) {
            childs[i] = create_tab_score_from_int(parents[i]->tab, matrix, N);
        }
        /*Mutate*/
        for(;i < IND_MUTATE;++i) {
            childs[i] = create_tab_score_from_int(parents[i%(int)(PERCENT_REPRODUCTION * NB_INDIVIDU / 100)]->tab, matrix, N);
            mutate(childs[i]->tab, N);
            childs[i]->score = scoring(childs[i]->tab, matrix, N);
        }
        /*Crossover*/
        for(;i < IND_CROSSOVER;++i) {
            get_2_random_parents(parents, &parent1, &parent2, (PERCENT_REPRODUCTION * NB_INDIVIDU / 100), matrix, N);
            order_crossover(parent1->tab, parent2->tab, child, N);
            childs[i] = create_tab_score_from_int(child, matrix, N);
        }
        /*New*/
        for(;i < IND_NEW;++i) {
            childs[i] = create_tab_score_from_points(tab1, matrix, N);
            init_random_tab(childs[i]->tab, N);
            childs[i]->score = scoring(childs[i]->tab, matrix, N);
        }

        quick_sort_children(childs, 0, NB_INDIVIDU-1);
        
        /*printf("100 premiers enfants triés : \n");
        for(i=0;i<100;++i) {
            print_array(childs[i]->tab, matrix);
        }*/
        copy_children_to_parents(parents, childs, NB_INDIVIDU);
        
        if(s % 30 == 0) {
            clear_window();
            for(i=255;i>=0;i -= 5) {
                fill_color_10(colors, i);
                show_path(tab1, parents[i]->tab, colors, 10, i==0, N);
            }
        }
        
        actualise_window();
        printf("Generation %d : Best score: %f Median: %f\n", s , parents[0]->score, parents[NB_INDIVIDU/2]->score);
        
        printf("Number of same score: %d\n", count_same_first_score(parents, NB_INDIVIDU));
    }
    printf("Best score: %f\n", parents[0]->score);
    printf("Median : %f\n", parents[NB_INDIVIDU/2]->score);
    print_points_from_array(parents[0]->tab, tab1, N);

    print_array_simple(parents[0]->tab, N);
    actualise_window();
    clear_window();
    actualise_window();
    fill_color_10(colors, 0);
    show_path(tab1, parents[0]->tab, colors, 10, 1, N);
    actualise_window();

    int i;
    for(i=0;i<NB_INDIVIDU; i++) {
        free_tab_score(parents[i]);
    }

    free(child);

    pause_keyboard();
    pause_keyboard();
    return 0;
}
