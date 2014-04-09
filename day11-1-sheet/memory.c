#include "bootpack.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

    // is CPU 386 or 486+
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT;    // 18 bit in EFLAGS reg 
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) 
    { 
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; 
	io_store_eflags(eflg);

	if (flg486 != 0)  // is 486 
    {   
	    cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;  //disable cache
	    store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;  // enable cache
		store_cr0(cr0);
	}

	return i;
}

/*  Compiler optimization cause error on this function 
//
unsigned int memtest_sub(unsigned int start, unsigned int end)
{
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for (i = start; i <= end; i += 0x1000)
    {
		p = (unsigned int *) (i + 0xffc);   // Just check the last 4 bytes
		old = *p;	        // record old value		
		*p = pat0;	        // write		 
		*p ^= 0xffffffff;	// revert
		if (*p != pat1)     // check result
        {	
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;	
		if (*p != pat0)      //
        {	
			goto not_memory;
		}
		*p = old;	        // restore
	}
	return i;
}
*/

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			
	man->maxfrees = 0;		
	man->lostsize = 0;		
	man->losts = 0;			
	return;
}

// Return summary of the free memory
unsigned int memman_total(struct MEMMAN *man)
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) 
    {
		if (man->free[i].size >= size) 
        {
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) 
            {
				man->frees--;
				for (; i < man->frees; i++) 
                {
					man->free[i] = man->free[i + 1]; 
				}
			}
			return a;
		}
	}
	return 0; 
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	for (i = 0; i < man->frees; i++) 
    {
		if (man->free[i].addr > addr) 
        {
			break;
		}
	}
	if (i > 0) 
    {
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			man->free[i - 1].size += size;
			if (i < man->frees) {
				if (addr + size == man->free[i].addr) {
					man->free[i - 1].size += man->free[i].size;
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; 
					}
				}
			}
			return 0; 
		}
	}
    // Can't merge with previous memory block
	if (i < man->frees) {
		if (addr + size == man->free[i].addr) {
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; 
		}
	}
    
    // Can't merge with following memory block
	if (man->frees < MEMMAN_FREES) 
    {
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; 
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; 
	}
	man->losts++;
	man->lostsize += size;
	return -1; 
}

// round down: 123 to 120, 0x12345678 to 0x12345000, i = i & 0xfffff000
// round up: Method 1: if(i & 0xfff) != 0 {i = (i & 0xfffff000) + 0x1000;}
//           Method 2: i = (i+0xfff) & 0xfffff000   
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size+0xfff) & 0xfffff000;
    a= memman_alloc(man, size);
	return a; 
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i;
    size = (size + 0xfff) & 0xfffff000;
    i = memman_free(man, addr, size);
    return i;
}
