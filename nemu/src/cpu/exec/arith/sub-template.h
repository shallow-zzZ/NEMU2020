#include "cpu/exec/template-start.h"

#define instr sub

#if DATA_BYTE == 2 || DATA_BYTE == 4

static void do_execute(){
	int8_t tmp = op_src->val;
	DATA_TYPE_S src = (DATA_TYPE_S)tmp;
	DATA_TYPE_S dest = op_dest->val;
	DATA_TYPE_S res = dest-src;
	if(op_dest->type == OP_TYPE_REG) { REG(op_dest->reg) = res; }
	else if(op_dest->type == OP_TYPE_MEM) { swaddr_write(op_dest->addr, DATA_BYTE, res); }
	// flags
	if(src<0 && dest>0 && res<0){
		cpu.OF = 1;
	} else if(src>0 && dest<0 && res>0){
		cpu.OF = 1;
	} else {
		cpu.OF = 0;
	}
	cpu.SF = (res<0)? 1:0;
	cpu.ZF = (!res)? 1:0;
	cpu.CF = 0;
	int i = 0, cnt = 0;
	for(i=0;i<8;i++){
		if(res & (1<<i)) cnt++;
	}
	cpu.PF = (cnt%2)? 0:1;
	print_asm_template2();
}

make_instr_helper(si2rm)

#endif

#include "cpu/exec/template-end.h"
