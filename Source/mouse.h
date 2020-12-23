#pragma once

class mice
{
 short int dividex, dividey, xtotalmickeys, ytotalmickeys;

 public:
 short int x, y, highx, highy, lowx, lowy;
 unsigned char left, right, buttons;
 unsigned short int l_presses, r_presses, l_releases, r_releases;
 picture background;
 picture cursor;

      mice();
 void initialize();
 void mouse_err();
 void getmove();
 void getbuttons();
 void movement();
 void update();
 void setrange(short int x1, short int y1, short int x2, short int y2);
 void putmouse(short int xpos, short int ypos);
 short int buttoncheck(unsigned short int check);
 void release();
};
