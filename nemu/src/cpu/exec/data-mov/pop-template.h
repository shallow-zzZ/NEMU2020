#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	OPERAND_W(op_src,MEM_R(reg_l(R_ESP),R_SS));
	MEM_W(reg_l(R_ESP),0,R_SS);
	reg_l(R_ESP)+=DATA_BYTE;
	print_asm_template1();
}

make_instr_helper(r);
#include "cpu/exec/template-end.h"
