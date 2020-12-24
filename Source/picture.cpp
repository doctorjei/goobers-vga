#include <alloc.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>
#include "picture.h"
#include "errors.h"

picture gameblock, goober, ns, nes, n, ne, nnw, n2, nne, ne2, font;

picture::picture()
{
 xlength = 320;
 ylength = 200;
 frames = 1;
 data = NULL;
}

picture::picture(short int x, short int y, short int frm, unsigned char *point)
{
 xlength = x;
 ylength = y;
 frames = frm;
 data = point;
}

void picture::addimagecol(short int x, short int y, short int frm, screen &dest, unsigned char fg, unsigned char bg)
{
 long record = xlength * ylength * frm;

 for (short int yindex=0; yindex<ylength; yindex++)
 {
  for (short int xindex=0; xindex<xlength; xindex++)
  {
   if (data[record] && x < 320 && y < 200)
   dest.setpixel(x, y, fg);
   if (!data[record] && bg)
   dest.setpixel(x, y, bg);
   x++;
   record++;
  }
  x = x - xlength;
  y++;
 }
}


void picture::addimage(short int x, short int y, short int frm, screen &dest, short int colormod)
{
 long record = xlength * ylength * frm;
 colormod <<= 4;

 for (short int yindex=0; yindex<ylength; yindex++)
 {
  for (short int xindex=0; xindex<xlength; xindex++)
  {
   if (data[record] && x < 320 && y < 200)
   dest.setpixel(x, y, data[record] + colormod);
   x++;
   record++;
  }
  x = x - xlength;
  y++;
 }
}

void picture::addimagesolid(short int x, short int y, short int frm, screen &dest)
{
 long record = xlength * ylength * frm;

 for (short int yindex=0; yindex<ylength; yindex++)
 {
  for (short int xindex=0; xindex<xlength; xindex++)
  {
   dest.setpixel(x, y, data[record]);
   x++;
   record++;
  }
  x = x - xlength;
  y++;
 }
}

void picture::getimage(short int x, short int y, short int frm, screen &source)
{
 long record = xlength * ylength * frm;

 for (short int yindex = 0; yindex < ylength; yindex++)
 {
  for (short int xindex = 0; xindex < xlength; xindex++)
  {
   data[record] = source.getpixel(x + xindex, y + yindex);
   record++;
  }
 }
}

void picture::rotateimage(short int times, picture &dest)
{
 dest.frames = frames;
 short int max, destn, source, framelength = xlength * ylength;
 long frameadd = 0;
 if (times == 0)
 {
  dest.xlength = xlength;
  dest.ylength = ylength;
  if (!dest.allocate()) memerr(10);
  memcpy(dest.data, data, framelength * frames);
 }
 else if (times == 1)
 {
  dest.xlength = ylength;
  dest.ylength = xlength;
  if (!dest.allocate()) memerr(10);
  max = ylength - 1;
  for (short int findex = 0; findex < frames; findex++)
  {
   source = 0;
   for (short int yindex = 0; yindex < ylength; yindex++)
   {
    destn = 0;
    for (short int xindex = 0; xindex < xlength; xindex++)
    {
     dest.data[max - yindex + destn + frameadd] =
     data[xindex + source + frameadd];
     destn += ylength;
    }
    source += xlength;
   }
   frameadd += framelength;
  }
 }
 else if (times == 2)
 {
  dest.xlength = xlength;
  dest.ylength = ylength;
  if (!dest.allocate()) memerr(10);
  for (short int findex = 0; findex < frames; findex++)
  {
   destn = 0;
   source = framelength;
   for (short int index = 0; index < framelength; index++)
   {
    source--;
    dest.data[destn + frameadd] = data[source + frameadd];
    destn++;
   }
   frameadd += framelength;
  }
 }
 else if (times == 3)
 {
  dest.xlength = ylength;
  dest.ylength = xlength;
  if (!dest.allocate()) memerr(10);
  max = framelength - ylength;
  for (short int findex = 0; findex < frames; findex++)
  {
   source = 0;
   for (short int yindex = 0; yindex < ylength; yindex++)
   {
    destn = max;
    for (short int xindex = 0; xindex < xlength; xindex++)
    {
     dest.data[yindex + destn + frameadd] =
     data[xindex + source + frameadd];
     destn -= ylength;
    }
    source += xlength;
   }
   frameadd += framelength;
  }
 }
}

void picture::copyto(picture &dest)
{
 _fmemcpy(dest.data, data, xlength * ylength);
}

short int picture::allocate()
{
 long datasize = (long)xlength * ylength * frames;

 if (datasize <= 0xFFFF)
   if ((data = (char *)malloc(datasize)) == NULL) return(0);
   else;
 else
   if ((data = (char *)farmalloc(datasize)) == NULL) return(0);
 return(1);
}

long picture::readimage(char *filename, long seek)
{
 long datasize;
 unsigned char *pnt;
 ifstream infile;

 infile.open(filename, ios::binary);
 if (!infile) file_error(filename);
 if (!infile.seekg(seek)) file_error(filename);
 if (!((infile.read((char *)&xlength, 2)) &&
       (infile.read((char *)&ylength, 2)) &&
       (infile.read((char *)&frames, 2))))
       {infile.close(); file_error(filename);}
 if (!(allocate())) {infile.close(); memerr(2);}
 pnt = data;
 datasize = (long) xlength * ylength * frames;
 while (datasize > 32767)
 {
  if (!(infile.read(pnt, 32767)))
       {infile.close(); file_error(filename);}
   datasize -= 32767;
   pnt = (pnt+32767);
 }
 if (!(infile.read(pnt, datasize)))
      {infile.close(); file_error(filename);}
 seek = infile.tellg();
 infile.close();
 return(seek);
}

void picture::freepicture()
{
 if ((long) xlength * ylength * frames <= 0xFFFF) free(data);
 else farfree(data);
}
