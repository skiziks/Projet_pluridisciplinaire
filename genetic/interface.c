#include <MLV/MLV_all.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "interface.h"

#define BASE_X 50
#define BASE_Y 50

#define MUL_X 400
#define MUL_Y 400

#define MIN_X 48.4
#define MIN_Y 2.5

void create_windows(void){
  MLV_create_window("Tests", NULL, WIDTH_WINDOWS_PIX, HEIGHT_WINDOWS_PIX);
}

void free_windows(void){
  MLV_free_window();
}

void actualise_window(void){
  MLV_actualise_window();
}

void clear_window(void){
  MLV_clear_window(MLV_rgba(0,0,0,255));
}

void pause_action(void){
  MLV_wait_milliseconds(WAIT_TIME_MILLISEC);
}

void pause_keyboard(void){
  MLV_wait_keyboard(NULL, NULL, NULL);
}


double x(double lat) {
  return BASE_X + (lat - MIN_X) * MUL_X;
}

double y(double lon) {
  return BASE_Y + (lon - MIN_Y) * MUL_Y;
}



void show_path(Point* tab[], int tab_points[], MLV_Color color[], int nb_colors, int with_circle, int N) {
  int i;
  int camion = 0;
  char str[5];
  for(i=1;i<N + NB_TRUCKS_MAX;++i) {
    Point* point = tab[z(tab_points[i])];
    if (with_circle) {
      MLV_draw_filled_circle(x(point->lat), y(point->lon), 5, point->id > 0 ?MLV_rgba(255,0,0,255):MLV_rgba(0,255,0,255));
      sprintf(str, "%d", point->id);
      MLV_draw_text(x(point->lat), y(point->lon), str, MLV_COLOR_WHITE);
    }
    
      MLV_draw_line(x(tab[z(tab_points[i-1])]->lat), y(tab[z(tab_points[i-1])]->lon),
                  x(point->lat), y(point->lon),
                  color[camion%nb_colors]);

    if (point->id <= 0 && i != 0 && i != N + NB_TRUCKS_MAX) {
      camion++;
    }
  }
  /*actualise_window();*/
  /*pause_action();*/
}