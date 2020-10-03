#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute(){
	DATA_TYPE_S src = op_src->val;
	DATA_TYPE_S dest = op_dest->val;
	DATA_TYPE_S res = dest + src + cpu.CF;
	DATA_TYPE ures = (DATA_TYPE) res;
	OPERAND_W(op_dest, ures);
	// flags
	if(src>0 && dest>0 && res<0){
		cpu.OF = 1;
	} else if(src<0 && dest<0 && res>0){
		cpu.OF = 1;
	} else {
		cpu.OF = 0;
	}
	cpu.SF = (res<0)? 1:0;
	cpu.ZF = !res;
	cpu.CF = (op_src->val > ures)? 1:0;
	int i = 0, cnt = 0;
	for(i=0;i<8;i++){
		if(res & (1<<i)) cnt++;
	}
	cpu.PF = (cnt%2)? 0:1;
	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(rm2r)
make_instr_helper(i2a)
make_instr_helper(r2rm)
make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
