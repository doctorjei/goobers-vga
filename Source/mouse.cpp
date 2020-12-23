#include "screen.h"
#include "mouse.h"

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
 ifstream cursorfile;
 long datasize;
 unsigned short int mousedata;

 asm {mov ax, 0x00; int 0x33; mov [mousedata], ax}
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
 cout << "Error initializing mouse. Exiting.\r\n";
 exit(1);
}

void mice::getmove()
{
 short int movex=0, movey=0;

 asm {
      mov ax, 0x0b
      int 0x33
      mov [movex], cx
      mov [movey], dx
     }

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

 asm {
      mov ax, 0x05; mov bx, 0; int 0x33; mov [new_l_press], bx;
      mov ax, 0x05; mov bx, 1; int 0x33; mov [new_r_press], bx;
      mov ax, 0x06; mov bx, 0; int 0x33; mov [new_l_release], bx;
      mov ax, 0x06; mov bx, 1; int 0x33; mov [new_r_release], bx;
     }

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
