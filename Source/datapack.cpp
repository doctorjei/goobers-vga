#include <conio.h>
#include <fstream.h>
#include <iostream.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void main()
{
 unsigned int x, y, frames, datalength, datatrack = 0;
 unsigned char character, total;
 char anifile[25], datfile[25], getfile[25], tmpstr[10];
 unsigned char *data;
 ofstream outfile;
 ifstream infile;

 data = (unsigned char *)malloc(64000);
 clrscr();
 cout << "animation: ";
 cin.get(anifile, 25, '\n');
 cin.ignore(80, '\n');
 cout << "x: ";
 cin.get(tmpstr, 10, '\n');
 cin.ignore(1024, '\n');
 x = atoi(tmpstr);
 cout << "y: ";
 cin.get(tmpstr, 10, '\n');
 cin.ignore(1024, '\n');
 y = atoi(tmpstr);
 cout << "frames: ";
 cin.get(tmpstr, 10, '\n');
 cin.ignore(1024, '\n');
 frames = atoi(tmpstr);
 strcpy(datfile, anifile);
 strcat(datfile, ".DAT");
 outfile.open(datfile, ios::binary);
 outfile.write((char *)&x, 2);
 outfile.write((char *)&y, 2);
 outfile.write((char *)&frames, 2);
 for (int index = 1; index <= frames; index++)
 {
  strcpy(getfile, anifile);
  strcat(getfile, itoa(index, tmpstr, 10));
  strcat(getfile, ".VGA");
  infile.open(getfile, ios::binary);
  datalength = x * y;
  infile.ignore(4);
  while(datalength)
  {
   if (!infile.get(character)) exit(1);
   outfile.write(&character, 1);
   datalength--;
  }
  infile.close();
 }
 outfile.close();
 cout << "Written to " << datfile << ".\n";
}