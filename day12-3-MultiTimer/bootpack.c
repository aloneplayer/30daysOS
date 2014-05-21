#include "bootpack.h"
#include <stdio.h>

void make_window8(unsigned char *buf, int xsize, int ysize, char *title);

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], mcursor[256];
	int mx, my, i;
    
    unsigned int memtotal;
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    
    //timer
    struct FIFO8 timerfifo1, timerfifo2, timerfifo3;
    char timerbuf1[8], timerbuf2[8], timerbuf3[8];
    struct TIMER *timer1 , *timer2, *timer3;
    
    //-- Layer management
    struct SHTCTL *shtctl;
    struct SHEET *sht_back, *sht_mouse;
    unsigned char *buf_back, buf_mouse[256];
    
    //-- Window
    struct SHEET *sht_win;
    unsigned char *buf_win;
    
	init_gdtidt();
	init_pic();
    // Set IF (interrupt flag to 1) to enable INT
    io_sti();
    
    init_palette();
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(mcursor, COL8_008484);
	
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
    
    io_out8(PIC0_IMR, 0xf8); /* (11111000) Open IRQ0(timer),IRQ1(keyboard) and IRQ2 connect to PIC1)*/
	io_out8(PIC1_IMR, 0xef); /* (11101111) Open IRQ 12 for mouse*/

    //--PIT (timer)
    init_pit();
    fifo8_init(&timerfifo1, 8, timerbuf1);
	timer1 = timer_alloc();
	timer_init(timer1, &timerfifo1, 1);
	timer_settime(timer1, 1000);
	fifo8_init(&timerfifo2, 8, timerbuf2);
	timer2 = timer_alloc();
	timer_init(timer2, &timerfifo2, 1);
	timer_settime(timer2, 300);
	fifo8_init(&timerfifo3, 8, timerbuf3);
	timer3 = timer_alloc();
	timer_init(timer3, &timerfifo3, 1);
	timer_settime(timer3, 50);
  
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
    sht_win   = sheet_alloc(shtctl);
    
    buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	buf_win   = (unsigned char *) memman_alloc_4k(memman, 160 * 68);
	
    sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); 
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	sheet_setbuf(sht_win, buf_win, 160, 68, -1); 
    
    init_screen8(buf_back, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(buf_mouse, 99);
    
    //--show window
    make_window8(buf_win, 160, 68, "window");
	putfonts8_asc(buf_win, 160, 24, 28, COL8_000000, "Welcome to");
	putfonts8_asc(buf_win, 160, 24, 44, COL8_000000, "  Haribote-OS!");
	
    //--
    sheet_slide(sht_back, 0, 0);
    
	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;
	
    sheet_slide(sht_mouse, mx, my);
    sheet_slide(sht_win, 80, 72);
	
    sheet_updown(sht_back,  0);
    sheet_updown(sht_win,  1);
	sheet_updown(sht_mouse, 2);
    
	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
    sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48);
	//sheet_refresh(shtctl);
    
    //-- Read keyboard and mouse input from the buffer
	for (;;) 
    {
        //-- Display timer count
        sprintf(s, "%010d", timerctl.count);
     	boxfill8(buf_win, 160, COL8_C6C6C6, 40, 28, 119, 43);
		putfonts8_asc(buf_win, 160, 40, 28, COL8_000000, s);
		sheet_refresh(sht_win, 40, 28, 120, 44);
                
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) 
             + fifo8_status(&timerfifo1)+ fifo8_status(&timerfifo2)+ fifo8_status(&timerfifo3)== 0) 
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
				sheet_refresh(sht_back, 0, 16, 16, 32);
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
					sheet_refresh(sht_back, 32, 16, 32 + 15 * 8, 32);
                    
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
                    // Mouse go outside of the screen
					if (mx > binfo->scrnx - 1) 
                    {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) 
                    {
						my = binfo->scrny - 1;
					}
                    sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 79, 15); 
					putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s); 
                    
					//-- Calling putblock8_8 will erase background
                    //putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); /* show cursor */   
                    sheet_refresh(sht_back, 0, 0, 80, 16);
					sheet_slide(sht_mouse, mx, my);                  
				}
			}
            else if (fifo8_status(&timerfifo1) != 0) 
            {
				i = fifo8_get(&timerfifo1); 
				io_sti();
				putfonts8_asc(buf_back, binfo->scrnx, 0, 64, COL8_FFFFFF, "10[sec]");
				sheet_refresh(sht_back, 0, 64, 56, 80);
			} 
            else if (fifo8_status(&timerfifo2) != 0) 
            {
				i = fifo8_get(&timerfifo2); 
				io_sti();
				putfonts8_asc(buf_back, binfo->scrnx, 0, 80, COL8_FFFFFF, "3[sec]");
				sheet_refresh(sht_back, 0, 80, 48, 96);
			} 
            else if (fifo8_status(&timerfifo3) != 0) 
            {
				i = fifo8_get(&timerfifo3);
				io_sti();
				if (i != 0) 
                {
					timer_init(timer3, &timerfifo3, 0); 
					boxfill8(buf_back, binfo->scrnx, COL8_FFFFFF, 8, 96, 15, 111);
				} 
                else 
                {
					timer_init(timer3, &timerfifo3, 1); 
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 8, 96, 15, 111);
				}
				timer_settime(timer3, 50);
				sheet_refresh(sht_back, 8, 96, 16, 112);
			}
		}      
    }
 }


void make_window8(unsigned char *buf, int xsize, int ysize, char *title)
{
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL8_000084, 3,         3,         xsize - 4, 20       );
	boxfill8(buf, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
	putfonts8_asc(buf, xsize, 24, 4, COL8_FFFFFF, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}


