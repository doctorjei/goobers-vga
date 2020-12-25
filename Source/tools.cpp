#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "mouse.h"
#include "tools.h"
#include "vga.h"
#include "errors.h"

extern mice mouse;
extern pal palette;
extern screen video;
extern screen vga1;
extern screen gameboard;
extern picture gameblock, goober, ns, nes, n, ne, nnw, n2, nne, ne2, font;
extern windows menu;

unsigned short *internalclock = (unsigned short *)0x0000046C;
unsigned short oldtime = *internalclock;

char *board;
short int sx = -1, sy = -1;
short int cplay = 0, nplay = 1;
char gameover = 0;

animation *firstani, *lastani, *ani;

animation::animation(picture *p, short int x1, short int y1, short int col, char val, char *chg)
{
 pic = p;
 x = x1; y = y1; color = col;
 curframe = 0;
 value = val;
 changeme = chg;
}

void quit(windows *win)
{
 char answerstring[2];

 video.wipe(0);
 cout << "\r\nQuit? [y/N] ";
 cin.get(answerstring, 2, '\n');
 cin.ignore(80, '\n');
 switch (answerstring[0])
 {
  case 'y':
  case 'Y': mode(0x03);
	    freememm();
	    exit(0);
 }
}

void closewindow(windows *win)
{
 win->down = 1;
}

void freememm()
{
 mouse.cursor.freepicture();
 mouse.background.freepicture();
 vga1.freescreen();
 gameboard.freescreen();
 menu.shutdown();
}

void fadeout()
{
 pal tmp;
 memcpy(tmp.colors, palette.colors, 768);
 for(short int rgb = 0; rgb < 64; rgb++)
 {
  for(short int cindex = 0; cindex < 256; cindex++)
  {
   if (tmp.colors[cindex].red > 0) tmp.colors[cindex].red--;
   if (tmp.colors[cindex].green > 0) tmp.colors[cindex].green--;
   if (tmp.colors[cindex].blue > 0) tmp.colors[cindex].blue--;
  }
  setpalette(tmp);
  retrace();
 }
}

void fadein()
{
 pal tmp;
 memset(tmp.colors, 0, 768);
 for(short int rgb = 0; rgb < 64; rgb++)
 {
  for(short int cindex = 0; cindex < 256; cindex++)
  {
   if (tmp.colors[cindex].red < palette.colors[cindex].red) tmp.colors[cindex].red++;
   if (tmp.colors[cindex].green < palette.colors[cindex].green) tmp.colors[cindex].green++;
   if (tmp.colors[cindex].blue < palette.colors[cindex].blue) tmp.colors[cindex].blue++;
  }
 retrace();
 setpalette(tmp);
 }
}

void setpalette(pal &setto)
{
 unsigned char col = 0;

 do
 {
  outp(0x03C8, col);
  outp(0x03C9, setto.colors[col].red);
  outp(0x03C9, setto.colors[col].green);
  outp(0x03C9, setto.colors[col].blue);
  col++;
 } while (col != 0);
}

void updatemovement()
{
 ani = firstani;
 while (ani != NULL)
 {
  ani->pic->addimage(ani->x, ani->y, ani->curframe, vga1, ani->color);
  ani->curframe++;
  if (ani->curframe == ani->pic->frames)
  {
   *(ani->changeme) = ani->value;
   animation *deleted = ani;

   if (ani->previous != NULL) ani->previous->next = ani->next;
   else firstani = firstani->next;
   if (ani->next != NULL)
    {ani->next->previous = ani->previous; ani = ani->next;}
   else lastani = ani = ani->previous;

   deleted->pic->freepicture();
   delete deleted->pic;
   delete deleted;
  }
  else ani = ani->next;
 }
}

void putgoober(short int x, short int y, short int fr, short int cm)
{
 gameblock.addimage(64+(x<<4)+(x<<3), 4+(y<<4)+(y<<3), 0, vga1, 0);
 if (board[x+(y<<3)] > -1)
 goober.addimage(64+(x<<4)+(x<<3), 4+(y<<4)+(y<<3), fr, vga1, cm);
}

void updategoobers()
{
 short int b0, b1;
 for(short int y = 0; y < 8; y++)
  for(short int x = 0; x < 8; x++)
  {
   b0 = x + (y << 3);
   b1 = x + (y << 3) + 64;
   if (board[b1] < 8) putgoober(x, y, board[b1], board[b0]);
   board[b1]++;
   if (board[b1] > 63) board[b1] = 0;
  }
}

void checkstep()
{
 short int bx, by;
 if (mouse.left == 1 && mouse.x > 63 && mouse.x < 256 &&
     mouse.y > 3 && mouse.y < 196)
 {
  bx = (mouse.x - 64)/24;
  by = (mouse.y - 4)/24;
  if (board[bx + (by << 3)] == cplay) {sx = bx; sy = by;}
  else if (sx > -1) if (!checksplit(bx, by)) checkjump(bx, by);
 }
}

short int checksplit(short int x, short int y)
{
 short int boardpos = x + (y << 3);
 short int x1, y1, x2, y2, placex, placey;
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 if (x-sx > -2 && x-sx < 2 && y-sy > -2 && y-sy < 2 && board[boardpos] == -1)
 {
  if (sx < x) {x1 = sx; x2 = x; placex = (sx << 3) + (sx << 4) + 64;}
  else {x1 = x; x2 = sx; placex = (x << 3) + (x << 4) + 64;}
  if (sy < y) {y1 = sy; y2 = y; placey = (sy << 3) + (sy << 4) + 4;}
  else {y1 = y; y2 = sy; placey = (y << 3) + (y << 4) + 4;}

  picture *newpic = new picture;
  ani = new animation(newpic, placex, placey, cplay, cplay, board+boardpos);
  lastani = firstani = ani; ani->next = ani->previous = NULL;
  if (sx == x)
   if (sy > y) n.rotateimage(0, *newpic);
   else n.rotateimage(2, *newpic);
  else if (sy == y)
   if (sx < x) n.rotateimage(1, *newpic);
   else n.rotateimage(3, *newpic);
  else if (sx < x)
   if (sy > y) ne.rotateimage(0, *newpic);
   else ne.rotateimage(1, *newpic);
  else if (sx > x)
   if (sy < y) ne.rotateimage(2, *newpic);
   else ne.rotateimage(3, *newpic);

  oldtime = *internalclock;
  while (board[boardpos] == -1)
  {
   if (*internalclock < oldtime || *internalclock > oldtime)
   {
    updategoobers();
    for(short int yi = y1; yi <= y2; yi++)
     for(short int xi = x1; xi <= x2; xi++)
      if (board[xi+(yi<<3)+64] > 4 || board[xi+(yi<<3)] == -1)
       putgoober(xi, yi, 3, board[xi+(yi<<3)]);
    gameblock.addimage(64+(sx<<4)+(sx<<3), 4+(sy<<4)+(sy<<3), 0, vga1, 0);
    gameblock.addimage(64+(x<<4)+(x<<3), 4+(y<<4)+(y<<3), 0, vga1, 0);
    updatemovement();
    oldtime = *internalclock;
   }
   mouse.getmove();
   mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
   mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
   mouse.getbuttons();
   if (mouse.right && mouse.left) menu.openwindow();
   mouse.update();
   vga1.copyto(video);
   mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
  }
  while(!placegoob(x1, y1, x2, y2));
  mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
  checkspawn(x, y);
  sx = -1;
  cplay++;
  if (cplay == nplay) cplay = 0;
  if (!checkplay()) gameover = 1;
  return 1;
 }
 else return 0;
}

void checkjump(short int x, short int y)
{
 short int boardpos = x + (y << 3);
 short int x1, y1, x2, y2, placex, placey;

 if (x-sx > -3 && x-sx < 3 && y-sy > -3 && y-sy < 3 && board[boardpos] == -1)
 {
  if (sx < x) {x1 = sx; x2 = x; placex = (sx << 3) + (sx << 4) + 64;}
  else {x1 = x; x2 = sx; placex = (x << 3) + (x << 4) + 64;}
  if (sy < y) {y1 = sy; y2 = y; placey = (sy << 3) + (sy << 4) + 4;}
  else {y1 = y; y2 = sy; placey = (y << 3) + (y << 4) + 4;}

  picture *newpic = new picture;
  ani = new animation(newpic, placex, placey, cplay, cplay, board+boardpos);
  lastani = firstani = ani; ani->next = ani->previous = NULL;

  if (sx == x)
   if (sy > y) n2.rotateimage(0, *newpic);
   else n2.rotateimage(2, *newpic);
  else if (sy == y)
   if (sx < x) n2.rotateimage(1, *newpic);
   else n2.rotateimage(3, *newpic);
  else if (sx < x)
  {if (sy > y)
   {if (sy - y > x - sx)
     nne.rotateimage(0, *newpic);
    else if (sy - y == x - sx)
     ne2.rotateimage(0, *newpic);
    else nnw.rotateimage(1, *newpic);}
   else if (sy < y)
   {if (y - sy > x - sx)
     nnw.rotateimage(2, *newpic);
    else if (y - sy == x - sx)
     ne2.rotateimage(1, *newpic);
    else nne.rotateimage(1, *newpic);}}
  else
   if (sy > y)
   {if (sy - y > sx - x)
     nnw.rotateimage(0, *newpic);
    else if (sy - y == sx - x)
     ne2.rotateimage(3, *newpic);
    else nne.rotateimage(3, *newpic);}
   else if (sy < y)
   {if (y - sy > sx - x)
     nne.rotateimage(2, *newpic);
    else if (y - sy == sx - x)
     ne2.rotateimage(2, *newpic);
    else nnw.rotateimage(3, *newpic);}

  oldtime = *internalclock;
  while (board[boardpos] == -1)
  {
   if (*internalclock < oldtime || *internalclock > oldtime)
   {
    updategoobers();
    for(short int yi = y1; yi <= y2; yi++)
     for(short int xi = x1; xi <= x2; xi++)
      if (board[xi+(yi<<3)+64] > 4 || board[xi+(yi<<3)] == -1)
       putgoober(xi, yi, 3, board[xi+(yi<<3)]);
    gameblock.addimage(64+(sx<<4)+(sx<<3), 4+(sy<<4)+(sy<<3), 0, vga1, 0);
    gameblock.addimage(64+(x<<4)+(x<<3), 4+(y<<4)+(y<<3), 0, vga1, 0);
    updatemovement();
    oldtime = *internalclock;
   }
   mouse.getmove();
   mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
   mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
   mouse.getbuttons();
   if (mouse.right && mouse.left) menu.openwindow();
   mouse.update();
   vga1.copyto(video);
   mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
  }
  board[sx + (sy << 3)] = -1;
  while(!placegoob(x1, y1, x2, y2));
  checkspawn(x, y);
  mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
  cplay++;
  if (cplay == nplay) cplay = 0;
  if (!checkplay()) gameover = 1;
  sx = -1;
 }
}

void checkspawn(short int xp, short int yp)
{
 short int x1, y1, x2, y2, plx, ply, spx = xp, spy = yp;
 picture *newpic = NULL;

 for (short int y = yp - 1; y < yp + 2; y++)
  for (short int x = xp - 1; x < xp + 2; x++)
  {
   if (y < 8 && y > -1 && x < 8 && x > -1)
   if (board[x+(y<<3)] != -1 && board[x+(y<<3)] != cplay)
   {
    if (xp < x) {x1 = xp; x2 = x; plx = (xp << 3) + (xp << 4) + 64;}
    else {x1 = x; x2 = xp; plx = (x << 3) + (x << 4) + 64;}
    if (yp < y) {y1 = yp; y2 = y; ply = (yp << 3) + (yp << 4) + 4;}
    else {y1 = y; y2 = yp; ply = (y << 3) + (y << 4) + 4;}

    newpic = new picture;
    ani = new animation(newpic, plx, ply, cplay, cplay, board+x+(y<<3));
    if (firstani == NULL)
     {lastani = firstani = ani; ani->next = ani->previous = NULL;}
    else
    {
     lastani->next = ani;
     ani->previous = lastani;
     lastani = ani;
     ani->next = NULL;
    }
    if (xp == x)
     if (yp > y) ns.rotateimage(0, *newpic);
     else ns.rotateimage(2, *newpic);
    else if (yp == y)
     if (xp < x) ns.rotateimage(1, *newpic);
     else ns.rotateimage(3, *newpic);
    else if (xp < x)
     if (yp > y) nes.rotateimage(0, *newpic);
     else nes.rotateimage(1, *newpic);
    else if (xp > x)
     if (yp < y) nes.rotateimage(2, *newpic);
     else nes.rotateimage(3, *newpic);
    spx = x;
    spy = y;
   }
  }
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 while (board[spx + (spy << 3)] != cplay)
 {
  if (*internalclock < oldtime || *internalclock > oldtime)
  {
   updategoobers();
   for(short int yi = yp - 1; yi < yp + 2; yi++)
    for(short int xi = xp - 1; xi < xp + 2; xi++)
     if (board[xi+(yi<<3)+64] > 4 || board[xi+(yi<<3)] == -1)
      if (yi < 8 && yi > -1 && xi < 8 && xi > -1)
       putgoober(xi, yi, 7, board[xi+(yi<<3)]);
   updatemovement();
   oldtime = *internalclock;
  }
  mouse.getmove();
  mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
  mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
  mouse.getbuttons();
  if (mouse.right && mouse.left) menu.openwindow();
  mouse.update();
  vga1.copyto(video);
  mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 }
 x1 = xp - 1; y1 = yp - 1; x2 = xp + 1; y2 = yp + 1;
 if (x1 < 0) x1 = 0; if (x2 > 7) x2 = 7;
 if (y1 < 0) y1 = 0; if (y2 > 7) y2 = 7;
 while(!placegoob(x1, y1, x2, y2));
}

void getinfo()
{
 char inputstr[25];
 gameover = 0;
 mode(0x10);
 cout << "Number of players: ";
 cin.get(inputstr, 25, '\n');
 cin.ignore(80, '\n');
 nplay = atoi(inputstr);
 if (nplay > 4) nplay = 4;
 if (nplay < 2) nplay = 2;
 mode(0x10);
}

short int placegoob(short int x1, short int y1, short int x2, short int y2)
{
 short int returnval = 0, b0;

 if (*internalclock < oldtime || *internalclock > oldtime)
 {
  for(short int y = y1; y <= y2; y++)
  for(short int x = x1; x <= x2; x++)
  {
   b0 = x + (y << 3);
   putgoober(x, y, 7, board[b0]);
   returnval = 1;
   oldtime = *internalclock;
  }
 }
 mouse.getmove();
 mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
 mouse.getbuttons();
 if (mouse.right && mouse.left) menu.openwindow();
 mouse.update();
 vga1.copyto(video);
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 return returnval;
}

short int checkplay()
{
 for(char y = 0; y < 8; y++)
  for(char x = 0; x < 8; x++)
   if (board[x + (y << 3)] == cplay)
   {
    char x1 = x - 2; char x2 = x + 3;
    char y1 = y - 2; char y2 = y + 3;
    if (x1 < 0) x1 = 0; if (x2 > 8) x2 = 8;
    if (y1 < 0) y1 = 0; if (y2 > 8) y2 = 8;
    for (char yi = y1; yi < y2; yi++)
     for (char xi = x1; xi < x2; xi++)
      if (board[xi + (yi << 3)] == -1) return 1;
   }
 return 0;
}

void fontwrite(screen *destin, unsigned char *text, unsigned char fg,
	       unsigned char bg, unsigned short x, unsigned short y)
{
 unsigned short xp = x, yp = y;
 while (*text != '\0')
 {
  font.addimagecol(xp, yp, *text - 32, *destin, fg, bg);
  xp = xp + 8;
  text = text+1;
  if (*text == '\r') { xp = x; yp = yp + 10; text = text+1;}
 }
}
