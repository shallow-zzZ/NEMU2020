#ifndef __HELPER_H__
#define __HELPER_H__

#include "nemu.h"
#include "cpu/decode/operand.h"
#include "../../../lib-common/x86-inc/mmu.h"

/* All function defined with 'make_helper' return the length of the operation. */
#define make_helper(name) int name(swaddr_t eip)

static inline uint32_t instr_fetch(swaddr_t addr, size_t len) {
	return swaddr_read(addr, len, R_CS);
}

/* Instruction Decode and EXecute */
static inline int idex(swaddr_t eip, int (*decode)(swaddr_t), void (*execute) (void)) {
	/* eip is pointing to the opcode */
	int len = decode(eip + 1);
	execute();
	return len + 1;	// "1" for opcode
}

/* LOAD SEGMENT'S BASE LIMIT INTO SEGMENT'S CACHE INVISIBLE PART*/
static inline void LoadScache(uint8_t sreg) {
	uint32_t addr = (cpu.sreg[sreg].index << 3) + cpu.gdtr.base;
	SegDesc segdesc;
	segdesc.low = lnaddr_read(addr,4);
	segdesc.high = lnaddr_read(addr+4,4);
	cpu.s_cache[sreg].base = (segdesc.base_31_24 << 24) | (segdesc.base_23_16 << 16) | (segdesc.base_15_0 << 0);
	cpu.s_cache[sreg].limit = (segdesc.limit_19_16 << 16) | segdesc.limit_15_0;
	if(segdesc.granularity) cpu.s_cache[sreg].limit <<= 12;
}

/* shared by all helper function */
extern Operands ops_decoded;

#define op_src (&ops_decoded.src)
#define op_src2 (&ops_decoded.src2)
#define op_dest (&ops_decoded.dest)


#endif
