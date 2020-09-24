#include "cpu/exec/template-start.h"

#define instr sub

#if DATA_BYTE == 2 || DATA_BYTE == 4

static void do_execute(){
	int8_t tmp = op_src->val;
	DATA_TYPE_S src = (DATA_TYPE_S)tmp;
	DATA_TYPE_S dest = op_dest->val;
	if(op_dest->type == OP_TYPE_REG) { REG(op_dest->reg) = dest-src; }
	else if(op_dest->type == OP_TYPE_MEM) { swaddr_write(op_dest->addr, DATA_BYTE, dest-src); }
	print_asm_template2();
}

make_instr_helper(si2rm)

#endif

#include "cpu/exec/template-end.h"
