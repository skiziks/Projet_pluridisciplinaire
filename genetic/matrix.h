#ifndef _MATRIX_
#define _MATRIX_


double** get_distance_meters_matrix_random(int size);
int** get_time_seconds_matrix_random(int size);
double** get_distance_meters_matrix_from_file(char* path);
int** get_time_seconds_matrix_from_file(char* path);


#endif