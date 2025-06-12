#include <stdio.h>
#include "output.h"

void write_output_to_file(TabScore *ts, double **dist_matrix, double **time_matrix, int N, const char *path) {
    FILE *file = fopen(path, "w");
    int i;
    int started = 0;
    for (i = 0; i < N + NB_TRUCKS_MAX; i++) {
        if (ts->tab[i] > 0) {
            if((ts->tab[i - 1] <= 0 || (i == 0 && ts->tab[i - 1] > 0)) && !started) {
                started = 1;
                fputc('0', file);
                fputc(' ', file);
            }
            fprintf(file, "%d", ts->tab[i]);
            if (ts->tab[i + 1] <= 0) {
                fputc(' ', file);
                fputc('0', file);
                fputc('\n', file);
                started = 0;
            } else {
                fputc(' ', file);
            }
        }
    }

    fclose(file);
}
