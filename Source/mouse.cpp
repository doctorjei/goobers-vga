#include <iostream>
#include "screen.h"
#include "mouse.h"
#include "vga.h"

extern screen vga1;
extern screen video;

mice mouse;

mice::mice()
{
 initialize();
 dividex = dividey = 5;
 highx = 319; highy = 199; lowx = 0; lowy = 0;
 y = 99; x = 159;
 xtotalmickeys = ytotalmickeys = left = right = 0;
 l_presses = r_presses = l_releases = r_releases = 0;
}

void mice::initialize()
{
 std::ifstream cursorfile;
 long datasize;
 unsigned short int mousedata;

 asm
 (
  "mov $0x00, %%ax;"
  "int $0x33;"
  "mov %%ax, %0;"
  : "=r"(mousedata) // Output variable / value
  : // Input variable / value (none)
  : "ax" // Clobbered register
 );

 if (!(mousedata==0xffff)) mouse_err();
 cursor.readimage("CURSOR.DAT", 0);
 background.xlength = cursor.xlength;
 background.ylength = cursor.ylength;
 background.frames = cursor.frames;
 background.allocate();
 getbuttons();
}

void mice::mouse_err()
{
 mode(0x03);
 std::cout << "Error initializing mouse. Exiting.\r\n";
 exit(1);
}

void mice::getmove()
{
 short int movex=0, movey=0;

 asm
 (
  "mov $0x0b, %%ax;"
  "int $0x33;"
  "mov %%cx, %[movex];"
  "mov %%dx, %[movey];"
  : [movex] "=&r" (movex), [movey] "=r" (movey) // Output variables (movex is "early clobber")
  : // Input values (none)
  : "ax", "cx", "dx"  // Clobbered registers
 );

 movex = movex + xtotalmickeys;
 movey = movey + ytotalmickeys;

 while(movex >= dividex)
 {
  movex = movex - dividex;
  if (x != highx) x = x + 1;
 }

 while(movex <= -dividex)
 {
  movex = movex + dividex;
  if (x != lowx) x = x - 1;
 }

 while(movey >= dividey)
 {
  movey = movey - dividey;
  if (y != highy) y = y + 1;
 }

 while(movey <= -dividey)
 {
  movey = movey + dividey;
  if (y != lowy) y = y - 1;
 }

 xtotalmickeys = movex;
 ytotalmickeys = movey;
}

void mice::getbuttons()
{
 short int new_l_press, new_r_press, new_l_release, new_r_release;

 asm
 (
  "mov $0x05, %%ax; mov $0, %%bx; int $0x33; mov %%bx, %[new_l_press];"
  "mov $0x05, %%ax; mov $1, %%bx; int $0x33; mov %%bx, %[new_r_press];"
  "mov $0x06, %%ax; mov $0, %%bx; int $0x33; mov %%bx, %[new_l_release];"
  "mov $0x06, %%ax; mov $1, %%bx; int $0x33; mov %%bx, %[new_r_release];"
  : [new_l_press] "=&r" (new_l_press), [new_r_press] "=&r" (new_r_press),
    [new_l_release] "=&r" (new_l_release), [new_r_release] "=r" (new_r_release)
  : // Input values (none)
  : "ax", "bx"  // Clobbered registers
 );

 l_presses = l_presses + new_l_press;
 r_presses = r_presses + new_r_press;
 l_releases = l_releases + new_l_release;
 r_releases = r_releases + new_r_release;

 if (l_presses > l_releases) left = 1; else left = 0;
 if (r_presses > r_releases) right = 1; else right = 0;

 buttons = 0x00;
 if (l_releases > 0)
 {
  buttons = buttons | 0x01;
 } else if (l_presses > 0) {buttons = buttons | 0x10;
 }
 if (r_releases > 0)
 {
  buttons = buttons | 0x02;
 } else if (r_presses > 0) buttons = buttons | 0x20;
}

void mice::movement()
{
 background.addimagesolid(x, y, 0, vga1);
 getmove();
 background.getimage(x, y, 0, vga1);
 cursor.addimage(x, y, 0, vga1, 0);
 getbuttons();
}

void mice::update()
{
 if (l_releases > 0) {l_releases--; l_presses--;}
 if (r_releases > 0) {r_releases--; r_presses--;}
}

void mice::setrange(short int x1, short int y1, short int x2, short int y2)
{
 lowx = x1; lowy = y1; highx = x2; highy = y2;
 if (x > highx) putmouse(highx, y);
 if (y > highy) putmouse(x, highy);
 if (x < lowx) putmouse(lowx, y);
 if (y < lowy) putmouse(x, lowy);
}

void mice::putmouse(short int xpos, short int ypos)
{
 background.addimagesolid(x, y, 0, vga1);
 x = xpos; y = ypos;
 background.getimage(x, y, 0, vga1);
 cursor.addimage(x, y, 0, vga1, 0);
}

short int mice::buttoncheck(unsigned short int check)
{
 unsigned char primary = check & 0xFF, secondary = check >> 8;
 if ((primary & buttons) == primary) return(1);
 if ((secondary & buttons) == secondary) return(2);
 return(0);
}

void mice::release()
{
 while(l_presses != l_releases || r_presses != r_releases)
 {movement(); vga1.copyto(video);}
}
