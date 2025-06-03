#include <stdio.h>
#include "output.h"

void write_output_to_file(TabScore *ts, double **dist_matrix, double **time_matrix, int N, const char *path) {
    FILE *file = fopen(path, "w");
    int i;
    for (i = 0; i < N + NB_TRUCKS_MAX; i++) {
        if (ts->tab[i] > 0) {
            fprintf(file, "%d", ts->tab[i]);
            if (ts->tab[i + 1] <= 0) {
                fputc('\n', file);
            } else {
                fputc(' ', file);
            }
        }
    }

    fclose(file);
}
