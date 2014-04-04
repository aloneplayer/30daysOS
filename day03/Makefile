TOOLPATH = ..\\_tools\\
MAKE     = $(TOOLPATH)make.exe
NASK     = $(TOOLPATH)nask.exe

default :
	$(MAKE) img

build : hhos.nas hhloader.nas Makefile
	$(NASK) hhloader.nas hhloader.bin hhloader.lst
	$(NASK) hhos.nas hhos.bin hhos.lst
    
img: build Makefile
	$(TOOLPATH)edimg.exe imgin:$(TOOLPATH)empty.tek wbinimg src:hhloader.bin len:512 from:0 to:0 copy from:hhos.bin to:@: imgout:hhos.img

run:	
	$(MAKE) -r img
	copy hhos.img ..\_tools\qemu\hhos.img
	$(MAKE) -C $(TOOLPATH)qemu
	
clean:
	del *.bin
	del *.lst
	del *.img