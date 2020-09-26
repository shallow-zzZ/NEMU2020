#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute(){
	DATA_TYPE res = op_dest->val + op_src->val + cpu.CF;
	OPERAND_W(op_dest, (DATA_TYPE)res);
	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
