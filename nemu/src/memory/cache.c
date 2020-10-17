#include "common.h"
#include "burst.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void dram_write(hwaddr_t, size_t, uint32_t);

uint32_t dram_cache(hwaddr_t, void *);

#define GRP_WIDTH 7
#define OFF_WIDTH 6
#define TAG_WIDTH (27 - GRP_WIDTH - OFF_WIDTH)
#define VALID_WIDTH 1

typedef struct{
	uint32_t valid : VALID_WIDTH;
	uint32_t tag   : TAG_WIDTH;
	uint8_t  blocks[64]; 
}Cache1;

typedef union {
	struct {	
		uint32_t offset : OFF_WIDTH;
		uint32_t grp : GRP_WIDTH;
		uint32_t tag : TAG_WIDTH;
	};
	uint32_t addr;
} cache_addr;

#define NR_GRP (1 << GRP_WIDTH)
#define NR_LINE 8

Cache1 L1[NR_GRP][NR_LINE];

void init_l1() {
	int i, j;
	for(i=0;i<NR_GRP;i++){
		for(j=0;j<NR_LINE;j++){
			L1[i][j].valid = 0;
		}
	}
}

static void l1_read(hwaddr_t addr, void *data) {
	//Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t offset = temp.offset;
	uint32_t grp = temp.grp;
	uint32_t tag = temp.tag;

	int i;
	for(i=0;i<NR_LINE;i++){
		if(L1[grp][i].valid && L1[grp][i].tag == tag){
			/* cache hit */
			printf("cache hit good!\n");
			memcpy(data,L1[grp][i].blocks+offset,BURST_LEN);
			return ;
		}
	}
	
	/* cache donot hit */
	printf("cache hit bad!\n");
	srand((unsigned)time(NULL));
	int vic = rand() % 8;
	dram_cache(addr, L1[grp][vic].blocks);
	L1[grp][vic].valid = 1;
	memcpy(data,L1[grp][vic].blocks+offset,BURST_LEN);
}

uint32_t L1_read(hwaddr_t addr, size_t len) {
	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];

	l1_read(addr, temp);
	if(offset + len > BURST_LEN) {
		/* in different cache block*/
		l1_read(addr + BURST_LEN, temp + BURST_LEN);
	}
	return unalign_rw(temp+offset, 4);
}

void L1_write(hwaddr_t addr, size_t len, uint32_t data) {
	dram_write(addr, len, data);
}
