#include "common.h"
#include "../../include/cpu/reg.h"

uint32_t L1_read(hwaddr_t, size_t);
void L1_write(hwaddr_t, size_t, uint32_t);

PTE read_tlb(lnaddr_t, bool*);
void update_tlb(PTE);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	return L1_read(addr,len) & (~0u >> ((4 - len) << 3));
	//return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	L1_write(addr, len, data);
	//dram_write(addr, len, data);
}

hwaddr_t page_translate(lnaddr_t addr) {
	if(cpu.cr0.protect_enable && cpu.cr0.paging) {
		p_lnaddr_t p_lnaddr;
		p_lnaddr.val = addr;
		PDE pde; PTE pte;

		/* READ TLB */
		bool flag = true;
		pte = read_tlb(addr, &flag);

		if(!flag) {
			printf("tlb not hit!\n");	
			hwaddr_t pde_addr = (cpu.cr3.page_directory_base << 12) + (p_lnaddr.dir << 2);
			pde.val = hwaddr_read(pde_addr, 4);
			Assert(pde.present, "%x", addr);
			hwaddr_t pte_addr = (pde.page_frame << 12) + (p_lnaddr.page << 2);
			pte.val = hwaddr_read(pte_addr, 4);
			Assert(pte.present, "%x",addr);
			update_tlb(pte);
		}
		hwaddr_t hwaddr = (pte.page_frame << 12) + p_lnaddr.offset;
		return hwaddr;
	}
	return addr;
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	if(((addr >> 20)+len >= (1<<12)) && cpu.cr0.protect_enable && cpu.cr0.paging) { 
		// data cross the page boundary
		Assert(0, "%x", addr);
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len); 
	}

//	hwaddr_t hwaddr = page_translate(addr);
//	return hwaddr_read(hwaddr, len); 
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif

	if(((addr >> 20)+len >= (1<<12)) && cpu.cr0.protect_enable && cpu.cr0.paging) { 
		// data cross the page boundary
		Assert(0, "%x", addr);
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_write(hwaddr, len, data);
	}

//	hwaddr_t hwaddr = page_translate(addr);
//	return hwaddr_write(hwaddr, len, data);
}

/* SEGMENT TRANSLATE swaddr->lnaddr */
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	if(cpu.cr0.protect_enable){
		assert(addr <= cpu.s_cache[sreg].limit);
		lnaddr_t lnaddr = cpu.s_cache[sreg].base + addr;
		return lnaddr;
	}
	return addr;
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

