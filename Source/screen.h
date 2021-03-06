#ifndef _SCREEN_H
#define _SCREEN_H

struct screen
{
 unsigned char *data;
 screen(unsigned char *point);

 void setpixel(short int x, short int y, unsigned char colnum);
 unsigned char getpixel(short int x, short int y);
 void wipe(unsigned char colnum);
 void copyto(screen &dest);
 void box(short int x1, short int y1, short int x2, short int y2, unsigned char col);
 void boxfill(short int x1, short int y1, short int x2, short int y2, unsigned char col);
 short int allocate();
 long readimage(char *filename, long seek);
 void freescreen();
};

struct color
{
 char red, green, blue;
};

struct pal
{
 color colors[256];
 long loadpalette(char *file, long seek);
};

#endif
