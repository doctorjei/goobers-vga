#pragma once

struct picture
{
 short int xlength;
 short int ylength;
 short int frames;
 unsigned char *data;

 picture();
 picture(short int x, short int y, short int frm, unsigned char *point);
 void addimage(short int x, short int y, short int frm, screen &dest, short int colormod);
 void addimagecol(short int x, short int y, short int frm, screen &dest, unsigned char fg, unsigned char bg);
 void addimagesolid(short int x, short int y, short int frm, screen &dest);
 void getimage(short int x, short int y, short int frm, screen &source);
 void rotateimage(short int times, picture &dest);
 void copyto(picture &dest);
 short int allocate();
 long readimage(char *filename, long seek);
 void freepicture();
}
