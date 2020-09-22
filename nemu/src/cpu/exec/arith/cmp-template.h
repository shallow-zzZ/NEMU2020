#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	print_asm("%x %s",op_src->val, op_src2->str);
	//DATA_TYPE_S res = op_src->val - op_src2->val;
	//cpu.OF = ;
	//cpu.SF = ;
	//cpu.ZF = ;
	//cpu.AF = ;
	//cpu.PF = ;
	//cpu.CF = ;
}

make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
