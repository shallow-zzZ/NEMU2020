#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat(movs_,SUFFIX)){
	DATA_TYPE src = swaddr_read(cpu.esi,DATA_BYTE);
	swaddr_write(cpu.edi, DATA_BYTE, src);
	if(!cpu.DF){
		cpu.esi += DATA_BYTE;
		cpu.edi += DATA_BYTE;
	} else {
		cpu.esi -= DATA_BYTE;
                cpu.edi -= DATA_BYTE;
	}
	print_asm(str(instr) str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
