#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

void init_pit(void)
{
    //0x2e9c=11932, INT per 10ms
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
    
    //
    timerctl.count = 0;
    timerctl.timeout =0;
	return;
}

void inthandler20(int *esp)
{
    io_out8(PIC0_OCW2, 0x60);	//processing finished
    timerctl.count ++;
    if(timerctl.timeout > 0)
    {
        timerctl.timeout--;
        if(timerctl.timeout == 0)
        {
            fifo8_put(timerctl.fifo, timerctl.data);
        }
    }
	return;
}

//disable INT and set timer , then enable INT
void settimer(unsigned int timeout, struct FIFO8 *fifo, unsigned char data)
{
    int eflags;
    eflags = io_load_eflags();
    io_cli();
    timerctl.timeout = timeout;
    timerctl.fifo = fifo;
    timerctl.data = data;
    io_store_eflags(eflags);
    return;
}