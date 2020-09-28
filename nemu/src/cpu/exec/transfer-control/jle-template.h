#include "cpu/exec/template-start.h"

#define instr jle

make_helper(concat(jle_i_,SUFFIX)){
	int len = concat(decode_i_,SUFFIX)(eip+1);
	DATA_TYPE_S src = op_src->val;
	print_asm(str(instr) "  %x",cpu.eip+ (int32_t)src+len+1);
	if(cpu.ZF || cpu.SF!=cpu.OF) {
		cpu.eip += (int32_t)src;
		if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	}
	return len+1;
}


#include "cpu/exec/template-end.h"
