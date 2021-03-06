#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_,SUFFIX)){
	DATA_TYPE src = swaddr_read(cpu.esi, DATA_BYTE, R_DS);
	concat(reg_,SUFFIX)(R_EAX) = src;
	if(!cpu.DF){
		cpu.esi += DATA_BYTE;
	} else {
                cpu.esi -= DATA_BYTE;
	}
	print_asm(str(instr) str(SUFFIX)" %x", src);
	return 1;
}

#include "cpu/exec/template-end.h"
