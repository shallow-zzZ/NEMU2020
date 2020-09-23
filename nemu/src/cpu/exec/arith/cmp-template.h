#include "cpu/exec/template-start.h"

#define instr cmp

/*
make_helper(concat(cmp_i2rm_,SUFFIX)){
	int len = concat(decode_rm_,SUFFIX)(eip+1);
	int8_t src1 = swaddr_read(eip + len +1,1);
	print_asm(str(instr) " " str(SUFFIX) " $0x%x %s",src1, op_src->str);
	DATA_TYPE_S src2 = op_src->val;
	DATA_TYPE_S res = src2 - src1;
	if(src2>0 && src1<0 && res<0){
		cpu.OF = 1;
	}else if(src2<0 && src1>0 && res>0){
		cpu.OF = 1;
	}else {
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
	return len+2;
}
*/

#if DATA_BYTE == 2 || DATA_BYTE ==4
static void do_execute(){
	DATA_TYPE_S src1 = op_dest->val;
	DATA_TYPE_S src2 = op_src->val;
	DATA_TYPE_S res = src1 - src2;
	if(src1>0 && src2<0 && res<0){
		cpu.OF = 1;
	}else if(src1<0 && src2>0 && res>0){
		cpu.OF = 1;
	}else {
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
	print_asm(str(instr) " %s %s",op_src->str, op_dest->str);
}

make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"
