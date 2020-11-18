#include <setjmp.h>

#include "common.h"

extern jmp_buf jbuf;
static inline void LoadScache(uint8_t sreg);
void raise_intr(uint8_t NO) {
	// push eflags
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.eflags, R_SS);

	// push cs
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.sreg[R_CS].val, R_SS);

	// push eip
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.eip, R_SS);
	
	uint32_t addr = cpu.idtr.base + (NO << 3);
	GateDesc gateDesc;
	gateDesc.low = lnaddr_read(addr,4);
	gateDesc.high = lnaddr_read(addr+4,4);
	cpu.sreg[R_CS].val = gateDesc.segment;
	LoadScache(R_CS);

	cpu.eip = (gateDesc.offset_31_16 << 16)|(gateDesc.offset_15_0);

	
	//uint32_t entry_addr = cpu.s_cache[R_CS].base + offset;
	
	longjmp(jbuf,1);
}
