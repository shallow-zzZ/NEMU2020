#include "cpu/exec/template-start.h"

#if DATA_BYTE == 1
make_helper(setne) {
	int len = decode_rm_b(eip+1);
	if(!cpu.ZF) {
		reg_b(op_src->reg) = 1;
	} else {
		reg_b(op_src->reg) = 0;
	}
	print_asm("set %s", op_src->str);
	return len+1;
}

make_helper(sete) {
	int len = decode_rm_b(eip+1);
	if(cpu.ZF) {
		reg_b(op_src->reg) = 1;
	} else {
		reg_b(op_src->reg) = 0;
	}
	print_asm("set %s", op_src->str);
	return len+1;
}
#endif

#include "cpu/exec/template-end.h"
