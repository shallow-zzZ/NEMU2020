#include "cpu/exec/template-start.h"

#define instr jmp

make_helper(concat(jmp_i_,SUFFIX)) {
	int len = concat(decode_i_,SUFFIX)(eip+1);
	DATA_TYPE_S src = op_src->val;
	cpu.eip += (int32_t)src;
	if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr) "  %x", cpu.eip + len + 1);
	return len+1;
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(jmp_rm_,SUFFIX)) {
	int len = concat(decode_rm_,SUFFIX)(eip+1);
	DATA_TYPE src = op_src->val;
	cpu.eip = src;
	if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr) "  %x", cpu.eip);
	cpu.eip -= (len+1);
	return len+1;
}
#endif

#if DATA_BYTE == 4
make_helper(ljmp) {
	int op1 = instr_fetch(eip+1,4);
	int op2 = instr_fetch(eip+5,2);
	cpu.eip = op1-7;
	cpu.cs.val = op2;
	return 7;
}
#endif

#include "cpu/exec/template-end.h"
