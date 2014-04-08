#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
/* init FIFO */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* Empty */
	fifo->flags = 0;  //reset the flag "no free space"
	fifo->p = 0; /* next write */
	fifo->q = 0; /* next read*/
	return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data)
/* put data into fifo */
{
	if (fifo->free == 0) {
		/* no free space */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	return 0;
}

int fifo8_get(struct FIFO8 *fifo)
/* Read data from FIFO*/
{
	int data;
	if (fifo->free == fifo->size) {
		/* FIFO is empty*/
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo8_status(struct FIFO8 *fifo)
/* How many data in the FIFO*/
{
	return fifo->size - fifo->free;
}