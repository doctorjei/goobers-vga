#include <fstream>
#include <cstring>
#include "screen.h"
#include "mouse.h"
#include "window.h"
#include "errors.h"

extern mice mouse;
extern screen video;
extern screen vga1;

windows menu, tools;

windows::windows()
{
 title = nullptr;
 object = firstobject = lastobject = nullptr;
 down = 0;
}

void windows::getdata(char *file, long seekpos)
{
 std::ifstream infile;
 char statuschar = 0;
 char tmp[1024] = "";

 infile.open(file, std::ifstream::binary);
 infile.seekg(seekpos);
 if (!infile) file_error(file);
 if (!(infile.read((char *) &x, 2) &&
       infile.read((char *) &y, 2) &&
       infile.get(tmp, 1024, '\0')))
       {infile.close(); file_error(file);}
 infile.ignore(1024, '\0');
 seekpos = infile.tellg();
 infile.close();
 if ((title = (char *)malloc(strlen(tmp) + 1)) == nullptr) memerr(5);
 strcpy(title, tmp);
 seekpos = graphic.readimage(file, seekpos);
 if (!background.allocate()) {infile.close(); file_error(file);}
 background.xlength = graphic.xlength;
 background.ylength = graphic.ylength;
 background.frames = 1;
 while (1)
 {
  infile.open(file, std::ifstream::binary);
  infile.seekg(seekpos);
  if (!infile.read(&statuschar, 1)) {infile.close(); file_error(file);}
  if (statuschar == '\0') break;
  object = new objects;
  object->next = object->previous = nullptr;
  if (firstobject == nullptr) firstobject = lastobject = object;
  else
  {
   lastobject->next = object;
   object->previous = lastobject;
   lastobject = object;
  }
  if (!(infile.read((char *) &object->xpos, 2) &&
	infile.read((char *) &object->ypos, 2) &&
	infile.read((char *) &object->execution, 2) &&
	infile.get(tmp, 1024, '\0')))
	{infile.close(); file_error(file);}
  infile.ignore(1024, '\0');
  seekpos = infile.tellg();
  infile.close();
  if ((object->text = (char *)malloc(strlen(tmp) + 1)) == nullptr) memerr(5);
  strcpy(object->text, tmp);
  seekpos = object->image.readimage(file, seekpos);
  object->currentframe = 0;
  object->pressed = 0;
 }
 infile.close();
 object = firstobject;
}

void windows::display(screen &dest)
{
 graphic.addimage(x, y, 0, dest, 0);
 object = firstobject;
 while (object != nullptr)
 {
  object->image.addimage(x + object->xpos, y + object->ypos,
  object->currentframe, dest, 0);
  object = object->next;
 }
}

void windows::shutdown()
{
 objects *nextobject;

 graphic.freepicture();
 background.freepicture();
 free(title);
 nextobject = object = firstobject;
 while (object != nullptr)
 {
  free(object->text);

  object->image.freepicture();
  nextobject = object->next;
  delete object;
  object = nextobject;
 }
 firstobject = object = lastobject = nullptr;
}

void windows::checkbuttons()
{
 object = firstobject;

 while (object != nullptr)
 {
  if (mouse.x >= object->xpos + x &&
      mouse.x < object->image.xlength + x + object->xpos &&
      mouse.y >= object->ypos + y &&
      mouse.y < object->image.ylength + y + object->ypos)
   {
   if (mouse.buttoncheck(object->execution) == 2) object->pressed = 1;
   else
    if (mouse.buttoncheck(object->execution) == 1)
    {
     object->pressed = -1;
     if (object->function != nullptr) object->function(this);
    } else object->pressed = 0;
   }
  else object->pressed = 0;
  checkobject(object);
  mouse.update();
  object = object->next;
 }
 object = firstobject;
 vga1.copyto(video);
}

void windows::checkobject(objects *obj)
{
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 if (obj->pressed != 0)
 {
  if (obj->pressed == 1)
   obj->image.addimage(x + obj->xpos, y + obj->ypos, 1, vga1, 0);
  else
  {
   obj->image.addimage(x + obj->xpos, y + obj->ypos, 0, vga1, 0);
   obj->pressed = 0;
  }
 }
 else obj->image.addimage(x + obj->xpos, y + obj->ypos, 0, vga1, 0);
 mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 mouse.cursor.addimage(mouse.x, mouse.y, 0, vga1, 0);
}

void windows::openwindow()
{
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 background.getimage(x, y, 0, vga1);
 display(vga1);
 mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 mouse.release();
 mouse.l_presses = mouse.l_releases = mouse.r_presses = mouse.r_releases = 0;

 while (down == 0)
 {
  mouse.movement();
  checkbuttons();
 }
 down = 0;
 mouse.background.addimagesolid(mouse.x, mouse.y, 0, vga1);
 background.addimagesolid(x, y, 0, vga1);
 mouse.background.getimage(mouse.x, mouse.y, 0, vga1);
 vga1.copyto(video);
}
