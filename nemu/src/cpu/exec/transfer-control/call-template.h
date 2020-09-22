#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_,SUFFIX)) {
	int len = concat(decode_i_,SUFFIX)(eip+1);
	if(DATA_BYTE == 2){
		reg_l(R_ESP) -= 2;
		swaddr_write(reg_l(R_ESP), 2, cpu.eip + len);
		cpu.eip = (cpu.eip + op_src->val) & 0x0000ffff;
	}else {
		reg_l(R_ESP) -= 4;
                swaddr_write(reg_l(R_ESP), 4, cpu.eip);
                cpu.eip = cpu.eip + op_src->val;
	}
	print_asm(str(instr) str(SUFFIX) " %x", cpu.eip);
	return len+1;
}

#include "cpu/exec/template-end.h"
