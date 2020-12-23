#pragma once

#include "picture.h"

class windows
{
 public:
 picture graphic, background;
 short int x, y, down;
 char *title;

 struct objects
 {
  picture image;
  unsigned short int xpos, ypos, execution, currentframe, pressed;
  char *text;
  void (*function)(windows *);
  objects *previous, *next;
 } *object, *firstobject, *lastobject;

 windows();
 void getdata(char *file, long seekpos);
 void display(screen &dest);
 void checkbuttons();
 void checkobject(objects *obj);
 void shutdown();
 void openwindow();
}
