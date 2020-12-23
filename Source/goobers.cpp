#include "vga.h"
#include "errors.h"
#include "screen.h"
#include "picture.h"
#include "mouse.h"
#include "window.h"
#include "tools.h"

void setdata();
void progloop();

extern mice mouse;
extern screen vga1;
extern screen video;
extern window menu;

extern unsigned short *internalclock;
unsigned short oldtime;

extern char *board;
extern short int sx, sy;
extern char gameover;

void main()
{
 randomize();
 getinfo();
 mode(0x13);
 setdata();
 fadeout();
 vga1.copyto(video);
 fadein();
 mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 while(1) progloop();
}

void progloop()
{
 if (gameover == 1) cout << "gameover";
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 if (*internalclock < oldtime || *internalclock > oldtime)
 {
  updategoobers();
  oldtime = *internalclock;
 }
 if (sx > -1) vga1.box((sx<<4)+(sx<<3)+64, (sy<<4)+(sy<<3)+4,
		       (sx<<4)+(sx<<3)+87, (sy<<4)+(sy<<3)+27, 15);
 mouse.getmove();
 mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
 mouse.getbuttons();
 vga1.copyto(video);
 if (sx > -1)
 {
  mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
  if (board[sx+(sy<<3)+64] < 8)
   putgoober(sx, sy, board[sx+(sy<<3)+64], board[sx+(sy<<3)]);
  else putgoober(sx, sy, 7, board[sx+(sy<<3)]);
  mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 }
 if (mouse.right && mouse.left) menu.openwindow();
 checkstep();
 mouse.update();
}
