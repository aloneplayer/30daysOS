TOOLPATH = ..\_tools\
MAKE     = $(TOOLPATH)make.exe
NASK     = $(TOOLPATH)nask.exe

default :
	$(MAKE) img

hhos.bin : hhos.nas Makefile
	$(NASK) hhos.nas hhos.bin hhos.lst

hhos.img : hhos.bin Makefile
	$(TOOLPATH)edimg.exe imgin:$(TOOLPATH)empty.tek wbinimg src:hhos.bin len:512 from:0 to:0 imgout:hhos.img

img:
	$(MAKE) -r hhos.img

run:	
	$(MAKE) -r hhos.img
	copy hhos.img ..\_tools\qemu\hhos.img
	$(MAKE) -C $(TOOLPATH)qemu
	
clean:
	del *.bin
	del *.lst
	del *.img