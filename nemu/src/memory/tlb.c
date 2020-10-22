#include "common.h"
#include "../../../lib-common/x86-inc/mmu.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NR_GRP 64

TLBE TLB[NR_GRP];

void init_tlb() {
	int i=0;
	for(; i<NR_GRP; i++) {
		TLB[i].valid = 0;
	}
}


PTE read_tlb(lnaddr_t addr, bool *success) {
	*success = true;
	PTE pte;
	pte.val = 0;
	int i=0;
	for(;i<64;i++) {
		/* TLB HIT */
		if(TLB[i].valid && TLB[i].tag == (addr >> 12)) {
			printf("tlb hit\n");
			pte.val = TLB[i].pte_cache.val;
			return pte;
		}
	}
	*success = false;
	return pte;
}

void update_tlb(PTE pte) {
	/* when not hit, update the tlb */
	srand((unsigned)time(NULL));
	int vic = rand()%64;
	TLB[vic].pte_cache.val = pte.val;
	TLB[vic].valid = 1;
}


