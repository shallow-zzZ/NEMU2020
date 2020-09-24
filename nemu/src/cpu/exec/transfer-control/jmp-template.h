#include "cpu/exec/template-start.h"

#define instr jmp

make_helper(concat(jmp_i_,SUFFIX)) {
	int len = concat(decode_i_,SUFFIX)(eip+1);
	cpu.eip += op_src->val;
	if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr) " %x", cpu.eip + len + 1);
	return len+1;
}

#include "cpu/exec/template-end.h"
