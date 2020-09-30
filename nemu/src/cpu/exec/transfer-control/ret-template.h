#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_,SUFFIX)){
	DATA_TYPE num = swaddr_read(reg_l(R_ESP),DATA_BYTE);
	cpu.eip = num;
	if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	reg_l(R_ESP) += DATA_BYTE;
	print_asm(str(instr));
	return 1;
}

#if DATA_BYTE == 2
make_helper(concat(ret_i_,SUFFIX)){
	//int len = concat(decode_i_,w)(eip+1);
	DATA_TYPE num = swaddr_read(reg_l(R_ESP),DATA_BYTE);
	cpu.eip = num;
	reg_l(R_ESP) += DATA_BYTE;
	//cpu.esp += 4;
	//print_asm_template();
	return 1;
}
#endif

#include "cpu/exec/template-end.h"
