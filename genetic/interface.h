#ifndef _INTERFACE_
#define _INTERFACE_

#include <MLV/MLV_all.h>
#include "matrix.h"
#include "genetic_utils.h"

#define HEIGHT_WINDOWS_PIX 600
#define WIDTH_WINDOWS_PIX 800


#define WAIT_TIME_MILLISEC 500



void create_windows(void);
void free_windows(void);
void actualise_window(void);
void pause_keyboard(void);
void clear_window(void);
void pause_action(void);
void show_path(Point* tab[], int tab_points[], MLV_Color color[], int nb_colors, int with_circle);

#endif
