#include "cpu/exec/template-start.h"

make_helper(concat(pusha_,SUFFIX)) {
	if(DATA_BYTE == 2){
		uint32_t tmp = reg_w(R_ESP);
		int i=0;
		for(;i<8;i++){
			reg_w(R_ESP) -= 4;
			swaddr_write(reg_w(R_ESP), 4, ((i==4)? tmp : reg_w(i)), R_SS);
		}
	}else {
		uint32_t tmp = reg_l(R_ESP);
		int i=0;
		for(;i<8;i++){
			reg_l(R_ESP) -= 4;
			swaddr_write(reg_l(R_ESP), 4, ((i==4)? tmp : reg_l(i)), R_SS);
		}
	}
	print_asm("pusha");
	return 1;
}

make_helper(concat(popa_,SUFFIX)) {
	if(DATA_BYTE == 2){
		int i=7;
		for(;i>=0;i--){
			if(i!=4) reg_w(i) = swaddr_read(reg_w(R_ESP), 4, R_SS);
			reg_w(R_ESP) += 4;
		}
	}else {
		int i=7;
		for(;i>=0;i--){
			if(i!=4) reg_l(i) = swaddr_read(reg_l(R_ESP), 4, R_SS);
			reg_l(R_ESP) += 4;
		}
	}
	print_asm("popa");
	return 1;
}

#include "cpu/exec/template-end.h"
