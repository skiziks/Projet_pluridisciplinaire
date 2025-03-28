#include <MLV/MLV_all.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "interface.h"

#define BASE_X 100
#define BASE_Y 100

void create_windows(void){
  MLV_create_window("Desk Calculator", NULL, WIDTH_WINDOWS_PIX, HEIGHT_WINDOWS_PIX);
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