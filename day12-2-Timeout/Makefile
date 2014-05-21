TOOLPATH = ..\\_tools\\
INCPATH  = .

MAKE     = $(TOOLPATH)make.exe
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
MAKEFONT = $(TOOLPATH)makefont.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com

OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj \
                dsctbl.obj int.obj fifo.obj debug.obj \
                keyboard.obj mouse.obj \
                memory.obj \
                sheet.obj \
                timer.obj

default :
	$(MAKE) img

hhloader.bin : hhloader.nas Makefile
	$(NASK) hhloader.nas hhloader.bin hhloader.lst
	
asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin hhloader.lst
 
bootpack.gas: bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c
    
bootpack.nas: bootpack.gas Makefile
	$(GAS2NASK) -o bootpack.gas bootpack.nas
    
bootpack.obj: bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst
    
hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

hhos.bin: asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb hhos.bin
    
img: hhloader.bin hhos.bin Makefile
	$(EDIMG) imgin:$(TOOLPATH)empty.tek wbinimg src:hhloader.bin len:512 from:0 to:0 copy from:hhos.bin to:@: imgout:hhos.img


# general rules

%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst


# 
run:	img
	copy hhos.img ..\_tools\qemu\hhos.img
	$(MAKE) -C $(TOOLPATH)qemu
	
install :
	$(MAKE) img
	$(IMGTOL) w a: hhos.img
clean:
	del *.bin
	del *.lst
	del *.img
	del *.gas
	del *.obj
	del bootpack.nas
	del bootpack.map
	del bootpack.bim
	del bootpack.hrb
