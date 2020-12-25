#include <fstream.h>
#include <alloc.h>
#include <dos.h>
#include <string.h>
#include "screen.h"
#include "errors.h"
#include "vga.h"

pal palette;
screen vga1(NULL), gameboard(NULL), video((unsigned char *)MK_FP(0xA000, 0));

screen::screen(unsigned char *point)
{
 data = point;
}

void screen::setpixel(short int x, short int y, unsigned char colnum)
{
 data[x + (y << 8) + (y << 6)] = colnum;
}

unsigned char screen::getpixel(short int x, short int y)
{
 return data[x + (y << 8) + (y << 6)];
}

void screen::wipe(unsigned char colnum)
{
 _fmemset(data, colnum, 64000);
}

void screen::copyto(screen &dest)
{
 retrace();
 memcpy(dest.data, data, 64000);
}

void screen::box(short int x1, short int y1, short int x2, short int y2, unsigned char col)
{
 short int tmp;
 if (x1 > x2) {tmp = x1; x1 = x2; x2 = tmp;}
 if (y1 > y2) {tmp = y1; y1 = y2; y2 = tmp;}
 for (short int yindex = y1; yindex <= y2; yindex++)
  {setpixel(x1, yindex, col); setpixel(x2, yindex, col);}
 for (short int xindex = x1; xindex <= x2; xindex++)
  {setpixel(xindex, y1, col); setpixel(xindex, y2, col);}
}

void screen::boxfill(short int x1, short int y1, short int x2, short int y2, unsigned char col)
{
 short int tmp;
 if (x1 > x2) {tmp = x1; x1 = x2; x2 = tmp;}
 if (y1 > y2) {tmp = y1; y1 = y2; y2 = tmp;}
 for (short int yindex = y1; yindex <= y2; yindex++)
  for (short int xindex = x1; xindex <= x2; xindex++)
  {setpixel(xindex, yindex, col);}
}

short int screen::allocate()
{
 if ((data = (char *)malloc(64000)) == NULL) return(0);
 return(1);
}

long screen::readimage(char *filename, long seek)
{
 ifstream infile;

 infile.open(filename, ios::binary);
 if (!infile) file_error(filename);
 if (!infile.seekg(seek)) file_error(filename);
 if (!(allocate())) {infile.close(); memerr(2);}
 if (!(infile.read(data, 32767))) {infile.close(); file_error(filename);}
 if (!(infile.read(data+32767, 31233))) {infile.close(); file_error(filename);}
 seek = infile.tellg();
 infile.close();
 return(seek);
}

void screen::freescreen()
{
 free(data);
}

long pal::loadpalette(char *file, long seek)
{
 ifstream infile;

 infile.open(file, ios::binary);
 if (!infile) {infile.close(); file_error(file);}
 infile.seekg(seek);
 if (!infile.read((unsigned char *)colors, 768))
 {infile.close(); file_error(file);}
 seek = infile.tellg();
 infile.close();
 return seek;
}

