#include "window.h"
#include "screen.h"
#include "picture.h"

struct animation
{
 picture *pic;
 short int x, y, color, curframe;
 char *changeme, value;
 animation *next, *previous;
 animation(picture *p, short int x1, short int y1, short int col, char val, char *chg);
} *firstani, *lastani, *ani;

void savefile(windows *win);
void loadfile(windows *win);
void quit(windows *win);
void closewindow(windows *win);
void freememm();
long loadpalette(char *file, long seek, pal *colors);
void fadeout();
void fadein();
void setpalette(pal *colors);
void aniupdate();
void updatemovement();
void putgoober(short int x, short int y, short int fr, short int cm);
void updategoobers();
void checkstep();
short int checksplit(short int x, short int y);
void checkjump(short int x, short int y);
void checkspawn(short int xp, short int yp);
void getinfo();
short int placegoob(short int x1, short int y1, short int x2, short int y2);
short int checkplay();
void fontwrite(screen *destin, unsigned char *text);
