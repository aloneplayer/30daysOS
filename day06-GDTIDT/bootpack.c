#include "bootpack.h"
#include <stdio.h>

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	char s[40], mcursor[256];
	int mx, my;

	init_gdtidt();
	init_pic();
    // Set IF (interrupt flag to 1)
    io_sti();
    
    init_palette();
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
	mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
    
    io_out8(PIC0_IMR, 0xf9); /* (11111001) Open IRQ 1 (keyboard) and IRQ 2 (connect to PIC 1)   */
	io_out8(PIC1_IMR, 0xef); /* (11101111) Open IRQ 12 */


	for (;;) {
		io_hlt();
	}
}