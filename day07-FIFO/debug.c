#include "bootpack.h"

void debug_print(int x, int y, unsigned char *s)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	putfonts8_asc(binfo->vram, binfo->scrnx, x, y, COL8_FF0000, s);
	
	return;
}
