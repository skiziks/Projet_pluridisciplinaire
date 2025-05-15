#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <MLV/MLV_all.h>
#include "interface.h"
#include "matrix.h"

#define NB_INDIVIDU 20000

#define PERCENT_KEEP 5
#define IND_KEEP (NB_INDIVIDU*PERCENT_KEEP/100)
#define PERCENT_MUTATE 25
#define IND_MUTATE (NB_INDIVIDU*PERCENT_MUTATE/100 + IND_KEEP)
#define PERCENT_CROSSOVER 40
#define IND_CROSSOVER (NB_INDIVIDU*PERCENT_CROSSOVER/100 + IND_MUTATE)
#define PERCENT_NEW 30
#define IND_NEW (NB_INDIVIDU*PERCENT_NEW/100 + IND_CROSSOVER)



double matrix[N][N];




int main() {
    srand(time(NULL));

    create_windows();

    Point* tab1[N];
    int child[N];
    MLV_Color colors[2];

    alloc_and_fill_tab(tab1);
    fill_matrix(tab1, matrix);

    TabScore* parents[NB_INDIVIDU];
    init_children(parents, NB_INDIVIDU, tab1, matrix);
    quick_sort_children(parents, 0, NB_INDIVIDU-1);
    TabScore* childs[NB_INDIVIDU];


    int s;
    int nb_gen = 1000;
    print_points_from_array(parents[0]->tab, tab1);
    clear_window();

    for(s=0;s<nb_gen;++s) {

        int i;
        TabScore* parent1;
        TabScore* parent2;

        /*Keep*/
        for(i=0;i < IND_KEEP;++i) {
            childs[i] = create_tab_score_from_int(parents[i]->tab, matrix);
        }
        /*Mutate*/
        for(;i < IND_MUTATE;++i) {
            childs[i] = create_tab_score_from_int(parents[i%(PERCENT_REPRODUCTION * N / 100)]->tab, matrix);
            mutate(childs[i]->tab);
            childs[i]->score = scoring(childs[i]->tab, matrix);
        }
        /*Crossover*/
        for(;i < IND_CROSSOVER;++i) {
            get_2_random_parents(parents, &parent1, &parent2, 300, matrix);
            order_crossover(parent1->tab, parent2->tab, child);
            childs[i] = create_tab_score_from_int(child, matrix);
        }
        /*New*/
        for(;i < IND_NEW;++i) {
            childs[i] = create_tab_score_from_points(tab1, matrix);
            init_random_tab(childs[i]->tab);
            childs[i]->score = scoring(childs[i]->tab, matrix);
        }

        quick_sort_children(childs, 0, NB_INDIVIDU-1);
        /*printf("100 premiers enfants triés : \n");
        for(i=0;i<100;++i) {
            print_array(childs[i]->tab, matrix);
        }*/
        copy_children_to_parents(parents, childs, NB_INDIVIDU);
        clear_window();
        for(i=255;i>=0;i -= 5) {
            colors[0] = MLV_rgba(255-i, 128-(i/2), 128-(i/2), 255);
            colors[1] = MLV_rgba(128-(i/2), 128-(i/2), 255-i, 255);
            show_path(tab1, parents[i]->tab, colors, 2, i==0);
        }
        actualise_window();
        printf("Generation %d : Best score: %f Median: %f\n", s , parents[0]->score, parents[NB_INDIVIDU/2]->score);
        
        printf("Number of same score: %d\n", count_same_first_score(parents, NB_INDIVIDU));
    }
    printf("Best score: %f\n", parents[0]->score);
    printf("Median : %f\n", parents[NB_INDIVIDU/2]->score);
    print_points_from_array(parents[0]->tab, tab1);

    print_matrix(matrix);
    actualise_window();
    clear_window();
    actualise_window();
    colors[0] = MLV_rgba(255, 128, 128, 255);
    colors[1] = MLV_rgba(128, 128, 255, 255);
    show_path(tab1, parents[0]->tab, colors, 2, 1);
    actualise_window();

    pause_keyboard();
    pause_keyboard();
    return 0;
}
