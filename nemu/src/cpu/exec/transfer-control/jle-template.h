#include "cpu/exec/template-start.h"

#define instr jle

make_helper(concat(jle_i_,SUFFIX)){
	int len = concat(decode_i_,SUFFIX)(eip+1);
	if(cpu.ZF || cpu.ZF!=cpu.OF){
		cpu.eip += op_src->val;
		if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm(str(instr) " %x", cpu.eip + len + 1);
	return len+1;
}


#include "cpu/exec/template-end.h"
