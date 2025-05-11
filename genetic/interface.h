#ifndef _INTERFACE_
#define _INTERFACE_

#define HEIGHT_WINDOWS_PIX 600
#define WIDTH_WINDOWS_PIX 800

#define WAIT_TIME_MILLISEC 500

#define N 80 

typedef struct _point {
    int id;
    double lat;
    double lon;
} Point;

/*Struct qui représente un tableau et son scoring*/
typedef struct _tabscore {
    int tab[N];
    double score;
} TabScore;

void create_windows(void);
void free_windows(void);
void actualise_window(void);
void clear_window(void);
void pause_action(void);
void show_path(Point* tab[], int tab_points[]);

#endif
