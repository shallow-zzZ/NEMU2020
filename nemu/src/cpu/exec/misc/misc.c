#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "cpu/intr.h"

void device_update();

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");
	return 1;
}

make_helper(intn) {
	int len = decode_i_b(eip+1); 
	raise_intr(op_src->val);
	print_asm("int3 0x%x",op_src->val);
	return len+1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(cli) {
	cpu.IF = 0;
	print_asm("cli");
	return 1;
}

make_helper(hlt) {
	while(!cpu.IF || !cpu.INTR) {
		device_update();
	}
	print_asm("hlt");
	return 1;
}
