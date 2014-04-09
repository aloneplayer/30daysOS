#include "bootpack.h"
#include <stdio.h>

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	char s[40], mcursor[256];
	int mx, my, i;
    
    unsigned int memtotal;
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    
    //-- Layer management
    struct SHTCTL *shtctl;
    struct SHEET *sht_back, *sht_mouse;
    unsigned char *buf_back, buf_mouse[256];
    
	init_gdtidt();
	init_pic();
    // Set IF (interrupt flag to 1) to enable INT
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
	io_out8(PIC1_IMR, 0xef); /* (11101111) Open IRQ 12 for mouse*/

    //--keyboard
    char keybuf[32];
    fifo8_init(&keyfifo,32, keybuf);
    init_keyboard();
    
    //-- Mouse
    char mousebuf[128];
    struct MOUSE_DEC mdec;
    fifo8_init(&mousefifo, 128, mousebuf);
    enable_mouse(&mdec);
    
    // Check memory
    memtotal = memtest(0x00400000, 0xbfffffff);
    memman_init(memman);
    memman_free(memman, 0x00001000, 0x0009e000);    
    memman_free(memman, 0x00400000, memtotal-0x00400000);

    //sprintf(s, "memory %dMB   free: %dKB", memtotal /(1024*1024), memman_total(memman)/1024);
    //putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

    //-- Sheet control
    shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back  = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); 
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(buf_mouse, 99);
	sheet_slide(shtctl, sht_back, 0, 0);
	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;
	sheet_slide(shtctl, sht_mouse, mx, my);
	sheet_updown(shtctl, sht_back,  0);
	sheet_updown(shtctl, sht_mouse, 1);
	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
    sheet_refresh(shtctl, sht_back, 0, 0, binfo->scrnx, 48);
	//sheet_refresh(shtctl);
    
    //-- Read keyboard and mouse input from the buffer
	for (;;) 
    {
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo)== 0) 
        {
			io_stihlt();
		} 
        else 
        {
         	if(fifo8_status(&keyfifo) !=0)
            {
                i = fifo8_get(&keyfifo);
                io_sti();
                sprintf(s, "%02X", i);
                boxfill8(buf_back, binfo->scrnx, COL8_008484,  0, 16, 15, 31);
				putfonts8_asc(buf_back, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				sheet_refresh(shtctl, sht_back, 0, 16, 16, 32);
            }
            else if(fifo8_status(&mousefifo) !=0)
            {
                i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) 
                {
                    sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) 
                    {
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) 
                    {
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) 
                    {
						s[2] = 'C';
					}
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
					putfonts8_asc(buf_back, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
					sheet_refresh(shtctl, sht_back, 32, 16, 32 + 15 * 8, 32);
                    
                    // Move mouse cursor
                    mx += mdec.x;
					my += mdec.y;
					if (mx < 0) 
                    {
						mx = 0;
					}
					if (my < 0) 
                    {
						my = 0;
					}
					if (mx > binfo->scrnx - 16) 
                    {
						mx = binfo->scrnx - 16;
					}
					if (my > binfo->scrny - 16) 
                    {
						my = binfo->scrny - 16;
					}
                    sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 79, 15); 
					putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s); 
                    
					//-- Calling putblock8_8 will erase background
                    //putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); /* show cursor */   
                    sheet_refresh(shtctl, sht_back, 0, 0, 80, 16);
					sheet_slide(shtctl, sht_mouse, mx, my);                  
				}
			}
		}      
    }
 }





