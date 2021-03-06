#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_,SUFFIX)) {
	int len = concat(decode_i_,SUFFIX)(eip+1);
	reg_l(R_ESP) -= DATA_BYTE;
        swaddr_write(reg_l(R_ESP), 4, cpu.eip + 4, R_SS);
	cpu.eip += op_src->val;
	if(DATA_BYTE == 2){
		cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm(str(instr) " %x", cpu.eip + 5);
	return len+1;
}

make_helper(concat(call_rm_,SUFFIX)) {
	int len = concat(decode_rm_,SUFFIX)(eip+1);
	reg_l(R_ESP) -= DATA_BYTE;
        swaddr_write(reg_l(R_ESP), 4, cpu.eip + len, R_SS);
	cpu.eip = op_src->val;
	if(DATA_BYTE == 2){
               cpu.eip = cpu.eip & 0x0000ffff;
        }
	cpu.eip -= (len+1);
	return len+1;
}


#include "cpu/exec/template-end.h"
