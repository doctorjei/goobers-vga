void mode(unsigned char screen_mode)
{
 asm
 (
  "mov $0x00, %%ah;"
  "mov %[screen_mode], %%al;"
  "int $0x10;"
  : // Output variables (none)
  : [screen_mode] "r" (screen_mode)
  : "ah", "al"
 );
}

void retrace()
{
 asm
 (
  "l1:"
   "mov $0x03DA, %%dx;"
   "in %%dx, %%al;"
   "and $0x08, %%al;"
   "jnz l1;"
  "l2:"
   "in %%dx, %%al;"
   "and $0x08, %%al;"
   "jz l2;"
   :
   :
   : "al", "dx"
 );
}
