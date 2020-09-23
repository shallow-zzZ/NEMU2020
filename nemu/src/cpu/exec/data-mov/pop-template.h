#include "cpu/exec/template-start.h"

#define instr pop

make_helper(concat(pop_r_,SUFFIX)){
	int len = concat(decode_r_,SUFFIX)(eip+1);
	DATA_TYPE_S num = swaddr_read(reg_l(R_ESP),DATA_BYTE);
	concat(reg_,SUFFIX)(op_src->reg) = num;
	reg_l(R_ESP) += DATA_BYTE;
	print_asm(str(instr) " %s", op_src->str);
	return len+1;
}

#include "cpu/exec/template-end.h"
