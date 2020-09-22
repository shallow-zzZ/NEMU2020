#include "cpu/exec/template-start.h"

#define instr ret

make_helper(ret){
	DATA_TYPE_S num = swaddr_read(reg_l(R_ESP),DATA_BYTE);
	cpu.eip = num;
	if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr));
	return 1;
}

#include "cpu/exec/template-end.h"
