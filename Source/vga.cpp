void mode(unsigned char screen_mode)
{
 asm {mov ah, 0x00; mov al, [screen_mode]; int 0x10;}
}

void retrace()
{
 l1: asm {
       mov dx, 0x03DA;
       in  al,dx;
       and al,0x08;
       jnz l1;
     }

 l2: asm {
       in  al,dx;
       and al, 0x08;
       jz  l2;
     }
}