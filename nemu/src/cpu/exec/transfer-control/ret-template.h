#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_,SUFFIX)){
	DATA_TYPE num = swaddr_read(reg_l(R_ESP),4, R_SS);
	reg_l(R_ESP) += 4;
	cpu.eip = num;
	//if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr) str(SUFFIX));
	return 1;
}

#if DATA_BYTE == 2
make_helper(concat(ret_i_,SUFFIX)){
	int len = concat(decode_i_,w)(eip+1);
	uint32_t num = swaddr_read(reg_l(R_ESP),4,R_SS);
	reg_l(R_ESP) += 4;
	cpu.eip = num - len;
	reg_l(R_ESP) += op_src->val;
	print_asm(str(instr) " %x", op_src->val);
	return len + 1;
}
#endif


#include "cpu/exec/template-end.h"
