#include <MLV/MLV_all.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "interface.h"

#define BASE_X 50
#define BASE_Y 50

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

void show_path(Point* tab[], int tab_points[], MLV_Color color[], int nb_colors, int with_circle) {
  int i;
  int camion = 0;
  for(i=1;i<N;++i) {
    Point* point = tab[tab_points[i]];
    if (point->id == 0 && i != 0 && i != N-1) {
      camion++;
    }
    if (with_circle) {
      MLV_draw_filled_circle(BASE_X + point->lat * 10, BASE_Y + point->lon * 10, 5, point->id?MLV_rgba(255,0,0,255):MLV_rgba(0,255,0,255));
    }
    
      MLV_draw_line(BASE_X + tab[tab_points[i-1]]->lat * 10, BASE_Y + tab[tab_points[i-1]]->lon * 10,
                  BASE_X + point->lat * 10, BASE_Y + point->lon * 10,
                  color[camion%nb_colors]);
  }
  /*actualise_window();*/
  /*pause_action();*/
}