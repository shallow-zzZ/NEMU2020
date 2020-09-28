#include "cpu/exec/template-start.h"

#define instr jbe

make_helper(concat(jbe_i_,SUFFIX)) {
	int len = concat(decode_i_,SUFFIX)(eip+1);
	if(cpu.CF || cpu.ZF) {
		DATA_TYPE_S src = op_src->val;
		cpu.eip += (int32_t)src;
		if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm(str(instr) "  %x",cpu.eip+len+1);
	return len+1;
}

#include "cpu/exec/template-end.h"
