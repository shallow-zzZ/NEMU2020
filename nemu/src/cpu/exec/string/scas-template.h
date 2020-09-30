#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_,SUFFIX)){
	DATA_TYPE src = swaddr_read(cpu.edi, DATA_BYTE);
	DATA_TYPE dest = concat(reg_,SUFFIX)(R_EAX);
	DATA_TYPE res = dest - src;
	if(dest>0 && src<0 && res<0){
		cpu.OF = 1;
	}else if(dest<0 && src>0 && res>0){
		cpu.OF = 1;
	}else {
		cpu.OF = 0;
	}
	cpu.SF = (res<0)? 1:0;
	cpu.ZF = (!res)? 1:0;
	cpu.CF = (op_dest->val < op_src->val)? 1:0;
	int i = 0, cnt = 0;
	for(i=0;i<8;i++){
		if(res & (1<<i)) cnt++;
	}
	cpu.PF = (cnt%2)? 0:1;
	if(!cpu.DF){
		cpu.edi += DATA_BYTE;
	} else {
                cpu.edi -= DATA_BYTE;
	}
	print_asm(str(instr) str(SUFFIX) " %x %x",dest, src);
	return 1;
}

#include "cpu/exec/template-end.h"
