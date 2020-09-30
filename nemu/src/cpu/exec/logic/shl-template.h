#include "cpu/exec/template-start.h"

#define instr shl

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;
	DATA_TYPE tmp = op_dest->val;
	int len = DATA_BYTE << 3;
	uint8_t count = src & 0x1f;
	dest <<= count;
	OPERAND_W(op_dest, dest);

	/* There is no need to update EFLAGS, since no other instructions 
	 * in PA will test the flags updated by this instruction.
	 */
	cpu.OF = (src>>(len-1))? 1:0;
	cpu.CF = 0;
	cpu.ZF = !tmp;
	cpu.SF = tmp >> (len-1);
	int i, cnt=0;
	for(i=0;i<8;i++){
		if(tmp&(1<<i)) cnt++;
	}
	cpu.PF = (cnt%2)? 0:1;
	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
