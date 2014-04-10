New-----------
Using map to keep the mouse cursor from flickering


Done-----------

Handle the mouse out of sceen.
	modify bootpack.c
	modify sheet_refreshsub() in sheet.c

Refactor sheet.c. 
	remove prameter "struct SHTCTL *ctl" from functions
	Modify sheet.c bootpack.h bootpack.c


Layer management
	Add sheet.c
        Modify Makefile


Refactor: add memory.c 
          Update Makefile, bootpack.h


Memory Management


Get memeory size

Refactor: add mouse.c and keyboard.c Modify Makefile


Using new data struct  MOUSE_DEC 
Parse mouse data to X, Y and Button


Read mouse data and reflactor


Add mosue control into int.c


Using FIFO : add fifo.c
Modify Makefile


Slipt files 

Using general rules in Makefile

Using bootpack.h