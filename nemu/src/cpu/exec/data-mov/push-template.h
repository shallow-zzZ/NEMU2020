#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	if(DATA_BYTE == 1) op_src->val = (int8_t) op_src->val;
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, op_src->val, op_src->sreg);
	print_asm("push %s", op_src->str);
}

make_instr_helper(r)
make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
