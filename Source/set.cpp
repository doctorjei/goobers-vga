#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "picture.h"
#include "window.h"
#include "tools.h"
#include "errors.h"
#include "vga.h"

extern pal *palette;
extern screen video;
extern screen vga1;
extern picture gameblock, goober, ns, nes, n, ne, nnw, n2, nne, ne2, font;
extern windows menu;

extern char *board;
extern int nplay;

extern animation *firstani, *lastani, *ani;

void setdata()
{
 if ((board = (char *)malloc(128)) == NULL) memerr(10);
 memset(board, -1, 128);
 short x, y;
 for (y = 0; y < 8; y++)
  for (x = 0; x < 8; x++)
   board[x + (y << 3)] = -1;

 if (nplay == 2)
 {board[0] = board[1] = board[8] = board[63] = board[62] = board[55] = 0;
  board[6] = board[7] = board[15] = board[48] = board[56] = board[57] = 1;}
 if (nplay > 2)
 {board[0] = board[1] = board[8] = 0;
  board[6] = board[7] = board[15] = 1;
  board[48] = board[56] = board[57] = 2;}
 if (nplay == 4)
  board[63] = board[62] = board[55] = 3;

 for (y = 0; y < 8; y++)
  for (x = 0; x < 8; x++)
   board[x + (y << 3) + 64] = random(32);

 firstani = lastani = ani = NULL;
 vga1.readimage("BOARD.DAT", 0);
 gameblock.readimage("BLOCK.DAT", 0);
 goober.readimage("GOOBER.DAT", 0);
 n.readimage("JUMPN.DAT", 0);
 n2.readimage("JUMPNN.DAT", 0);
 nne.readimage("JUMPNNE.DAT", 0);
 nnw.readimage("JUMPNNW.DAT", 0);
 ne.readimage("JUMPNE.DAT", 0);
 ne2.readimage("JUMPNE2.DAT", 0);
 ns.readimage("JUMPNS.DAT", 0);
 nes.readimage("JUMPNES.DAT", 0);
 font.readimage("FONT.DAT", 0);
 menu.getdata("WINDOW.DAT", 0);
 menu.object = menu.firstobject; menu.object->function = NULL;//save
 menu.object = menu.object->next; menu.object->function = NULL;//load
 menu.object = menu.object->next; menu.object->function = NULL;//new
 menu.object = menu.object->next; menu.object->function = quit;
 menu.object = menu.object->next; menu.object->function = closewindow;
 menu.object = menu.firstobject;
 loadpalette("PALETTE.DAT", 0, palette);
 for (y = 0; y < 8; y++)
  for (x = 0; x < 8; x++)
   putgoober(x, y, 7, board[x + (y << 3)]);
 retrace();
 video.wipe(255);
}
