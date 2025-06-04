#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <MLV/MLV_all.h>
#include "interface.h"
#include "matrix.h"
#include "input.h"
#include "output.h"

#define NB_INDIVIDU 20000
#define NB_ENVIRONMENTS 5
#define INDIVIDUALS_PER_ENVIRONMENT (NB_INDIVIDU / NB_ENVIRONMENTS)

#define PERCENT_REPRODUCTION 1

#define PERCENT_KEEP 1
#define IND_KEEP (INDIVIDUALS_PER_ENVIRONMENT * PERCENT_KEEP / 100)
#define PERCENT_MUTATE 40
#define IND_MUTATE (INDIVIDUALS_PER_ENVIRONMENT * PERCENT_MUTATE / 100 + IND_KEEP)
#define PERCENT_CROSSOVER 39
#define IND_CROSSOVER (INDIVIDUALS_PER_ENVIRONMENT * PERCENT_CROSSOVER / 100 + IND_MUTATE)
#define PERCENT_NEW 20
#define IND_NEW (INDIVIDUALS_PER_ENVIRONMENT * PERCENT_NEW / 100 + IND_CROSSOVER)

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

void print_info(TabScore *ts, double **dist_matrix, double **time_matrix, int N) {
    int i;
    int truck_time_s = 0;
    for(i=0;i<N+NB_TRUCKS_MAX;i++) {
        truck_time_s += distance(ts->tab[i], ts->tab[i+1], time_matrix);
        if(ts->tab[i] > 0) {
            printf("%d ", ts->tab[i]);
            if(ts->tab[i+1] <= 0) {
                printf("Total time : %dh%dm%ds\n", truck_time_s/3600, (truck_time_s%3600)/60, truck_time_s%60);
                truck_time_s = 0;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <time_limit_in_seconds>\n", argv[0]);
        return 1;
    }

    int time_limit = atoi(argv[1]);
    if (time_limit <= 0) {
        fprintf(stderr, "Invalid time limit. Please provide a positive integer.\n");
        return 1;
    }

    srand(time(NULL));

    create_windows();

    int N = count_matrix_size("../livraison85/matrice_durees_s.csv") - 1;

    double **time_matrix = get_time_seconds_matrix_from_file("../livraison85/matrice_durees_s.csv");
    double **matrix = get_distance_meters_matrix_from_file("../livraison85/matrice_distances_m.csv");
    Point** points = get_points_tab_from_file("../livraison85/pharmacies_etudiees.csv");

    Point** tab1 = copy_with_trucks(points, N);
    free(points);
    int *child = malloc((N + NB_TRUCKS_MAX + 1) * sizeof(int));
    MLV_Color colors[10];

    TabScore* environments[NB_ENVIRONMENTS][INDIVIDUALS_PER_ENVIRONMENT];
    TabScore* children[NB_ENVIRONMENTS][INDIVIDUALS_PER_ENVIRONMENT];

    // Initialize environments and children
    for (int env = 0; env < NB_ENVIRONMENTS; ++env) {
        for (int i = 0; i < INDIVIDUALS_PER_ENVIRONMENT; ++i) {
            environments[env][i] = create_tab_score_from_points(tab1, matrix, time_matrix, N);
            init_random_tab(environments[env][i]->tab, N);
            environments[env][i]->score = scoring(environments[env][i]->tab, matrix, time_matrix, N);
        }
        quick_sort_children(environments[env], 0, INDIVIDUALS_PER_ENVIRONMENT - 1);
    }

    time_t start_time = time(NULL);
    int s = 0;

    while (difftime(time(NULL), start_time) < time_limit) {
        for (int env = 0; env < NB_ENVIRONMENTS; ++env) {
            TabScore* parent1;
            TabScore* parent2;

            int i = 0;

            // Keep
            for (; i < IND_KEEP; ++i) {
                children[env][i] = create_tab_score_from_int(environments[env][i]->tab, matrix, time_matrix, N);
            }

            // Mutate
            for (; i < IND_MUTATE; ++i) {
                children[env][i] = create_tab_score_from_int(environments[env][i % (int)(PERCENT_REPRODUCTION * INDIVIDUALS_PER_ENVIRONMENT / 100)]->tab, matrix, time_matrix, N);
                mutate(children[env][i]->tab, N);
                children[env][i]->score = scoring(children[env][i]->tab, matrix, time_matrix, N);
            }

            // Crossover
            for (; i < IND_CROSSOVER; ++i) {
                get_2_random_parents(environments[env], &parent1, &parent2, (PERCENT_REPRODUCTION * INDIVIDUALS_PER_ENVIRONMENT / 100), matrix, N);
                order_crossover(parent1->tab, parent2->tab, child, N);
                children[env][i] = create_tab_score_from_int(child, matrix, time_matrix, N);
            }

            // New
            for (; i < IND_NEW; ++i) {
                children[env][i] = create_tab_score_from_points(tab1, matrix, time_matrix, N);
                init_random_tab(children[env][i]->tab, N);
                children[env][i]->score = scoring(children[env][i]->tab, matrix, time_matrix, N);
            }

            quick_sort_children(children[env], 0, INDIVIDUALS_PER_ENVIRONMENT - 1);
            copy_children_to_parents(environments[env], children[env], INDIVIDUALS_PER_ENVIRONMENT);
        }

        if (s % 30 == 0) {
            clear_window();
            for (int env = 0; env < NB_ENVIRONMENTS; ++env) {
                for (int i = 50; i >= 0; i -= 1) {
                    fill_color_10(colors, i * 5);
                    show_path(tab1, environments[env][i]->tab, colors, 10, i == 0, N, 300 * (env % 5), 400 * (env / 5));
                }
            }
        }

        actualise_window();
        printf("Generation %d : Best scores: ", s);
        for (int env = 0; env < NB_ENVIRONMENTS; ++env) {
            printf("Env %d: %f ", env, environments[env][0]->score);
        }
        printf("\n");

        s++;
    }

    printf("Best scores per environment:\n");

    double min_score = environments[0][0]->score;
    int min = 0;

    clear_window();
    for (int env = 0; env < NB_ENVIRONMENTS; ++env) {
        if (environments[env][0]->score < min_score) {
            min_score = environments[env][0]->score;
            min = env;
        }
        fill_color_10(colors, 0);
        show_path(tab1, environments[env][0]->tab, colors, 10, 1, N, 300 * (env % 5), 400 * (env / 5));
        printf("Environment %d: Best score: %f\n", env, environments[env][0]->score);
        print_array(environments[env][0]->tab, matrix, time_matrix, N);
    }

    actualise_window();

    printf("\n\nBest env : %d\n\n", min);

    print_info(environments[min][0], matrix, time_matrix, N);

    const char *output_path = "../output.txt";
    write_output_to_file(environments[min][0], matrix, time_matrix, N, output_path);
    printf("Best environment's output written to %s\n", output_path);

    int i;
    for (int env = 0; env < NB_ENVIRONMENTS; ++env) {
        for (i = 0; i < INDIVIDUALS_PER_ENVIRONMENT; i++) {
            free_tab_score(environments[env][i]);
        }
    }

    free(child);

    pause_keyboard();
    pause_keyboard();
    return 0;
}
