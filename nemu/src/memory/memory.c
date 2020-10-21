#include "common.h"
#include "../../include/cpu/reg.h"

uint32_t L1_read(hwaddr_t, size_t);
void L1_write(hwaddr_t, size_t, uint32_t);

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
	printf("%d\n", cpu.cr0.paging);
	if(cpu.cr0.protect_enable && cpu.cr0.paging) {
		p_lnaddr_t p_lnaddr;
		p_lnaddr.val = addr;
		PDE pde; PTE pte;
		hwaddr_t pde_addr = (cpu.cr3.page_directory_base << 12) + (p_lnaddr.dir << 2);
		pde.val = hwaddr_read(pde_addr, 4);
		assert(pde.present);
		hwaddr_t pte_addr = (pde.page_frame << 12) + (p_lnaddr.page << 2);
		pte.val = hwaddr_read(pte_addr, 4);
		assert(pte.present);
		hwaddr_t hwaddr = (pte.page_frame << 12) + p_lnaddr.offset;
		return hwaddr;
	}
	return addr;
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	if((addr & 0x0fff)+len >= (1<<12)) { // data cross the page boundary
		assert(0);
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len); 
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
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

