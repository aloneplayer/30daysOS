#include "bootpack.h"
#include <stdio.h>

void init_pic(void)
{
	io_out8(PIC0_IMR,  0xff  ); /* ȫ�Ƥθ���z�ߤ��ܤ������ʤ� */
	io_out8(PIC1_IMR,  0xff  ); /* ȫ�Ƥθ���z�ߤ��ܤ������ʤ� */

	io_out8(PIC0_ICW1, 0x11  ); /* ���å��ȥꥬ��`�� */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7�ϡ�INT20-27���ܤ��� */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1��IRQ2�ˤƽӾA */
	io_out8(PIC0_ICW4, 0x01  ); /* �Υ�Хåե���`�� */

	io_out8(PIC1_ICW1, 0x11  ); /* ���å��ȥꥬ��`�� */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15�ϡ�INT28-2f���ܤ��� */
	io_out8(PIC1_ICW3, 2     ); /* PIC1��IRQ2�ˤƽӾA */
	io_out8(PIC1_ICW4, 0x01  ); /* �Υ�Хåե���`�� */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1�����ȫ�ƽ�ֹ */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 ȫ�Ƥθ���z�ߤ��ܤ������ʤ� */

	return;
}

#define PORT_KEYDAT		0x0060
struct FIFO8 keyfifo;

// For keyboard
void inthandler21(int *esp)
{
	unsigned char data;
    io_out8(PIC0_OCW2, 0x61);	/*Notify IRQ-01 was handled */
	data = io_in8(PORT_KEYDAT);
    fifo8_put(&keyfifo, data);
    
    return;
}

// For mouse
struct FIFO8 mousefifo;
void inthandler2c(int *esp)
{
	unsigned char data;
   	io_out8(PIC0_OCW2, 0x64);	/*Notify IRQ-12 (No.4 in PIC 1)was handled */
    io_out8(PIC0_OCW2, 0x62);	/*Notify IRQ-02 was handled */

    data = io_in8(PORT_KEYDAT);
	fifo8_put(&mousefifo, data);
}

void inthandler27(int *esp)
/* PIC0����β���ȫ����z�ߌ��� */
/* Athlon64X2�C�ʤɤǤϥ��åץ��åȤζ��Ϥˤ��PIC�γ��ڻ��r�ˤ��θ���z�ߤ�1�Ȥ��������� */
/* ���θ���z�߄I���v���ϡ����θ���z�ߤˌ����ƺΤ⤷�ʤ��Ǥ���^���� */
/* �ʤ��Τ⤷�ʤ��Ƥ����Σ�
	��  ���θ���z�ߤ�PIC���ڻ��r��늚ݵĤʥΥ����ˤ�äưk��������ΤʤΤǡ�
		�ޤ���˺Τ��I���Ƥ���Ҫ���ʤ���									*/
{
	io_out8(PIC0_OCW2, 0x67); /* IRQ-07�ܸ����ˤ�PIC��֪ͨ */
	return;
}