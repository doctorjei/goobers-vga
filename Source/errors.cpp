#include <iostream.h>
#include <stdlib.h>
#include "vga.h"

void memerr(short int errornum)
{
 mode(0x03);
 cout << "There was an error (" << errornum << ") allocating memory. Exiting.\r\n";
 exit(1);
}

void file_error(char *filename)
{
 mode(0x03);
 cout << "There was an error opening \"" << filename << "\". Exiting.\r\n";
 exit(1);
}
