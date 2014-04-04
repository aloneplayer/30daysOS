void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int  io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, us)

void HariMain(void)
{
    int i;
    
    for(i = 0xa0000 ; i<0xaffff; i ++)
    {
        // write_mem8(i, 15); white screen
        write_mem8(i, i& 0xf);   // color bar
    }

    for(;;)
        io_hlt(); //execute io_hlt in naskfunc.nac    
}