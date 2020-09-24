#include "cpu/exec/template-start.h"

#define instr leave 

make_helper(concat(leave_,SUFFIX)) {
	reg_l(R_ESP) = reg_l(R_EBP);
	DATA_TYPE_S src = swaddr_read(reg_l(R_ESP), DATA_BYTE);
	reg_l(R_EBP) = src;
	reg_l(R_ESP) += DATA_BYTE;
	print_asm(str(instr));
	return 1;
}

#include "cpu/exec/template-end.h"
