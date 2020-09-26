#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute(){
	DATA_TYPE_S src = op_src->val;
	DATA_TYPE_S dest = op_dest->val;
	DATA_TYPE_S res = dest + src + cpu.CF;
	OPERAND_W(op_dest, (DATA_TYPE)res);
	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
