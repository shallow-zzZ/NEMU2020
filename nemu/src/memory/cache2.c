#include "common.h"
#include "burst.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void cache_dram(hwaddr_t , void *); 
uint32_t dram_cache(hwaddr_t, void *);

#define GRP_WIDTH 12
#define OFF_WIDTH 6
#define TAG_WIDTH (27 - GRP_WIDTH - OFF_WIDTH)
#define VALID_WIDTH 1
#define DIRTY_WIDTH 1


typedef struct{
	uint32_t valid : VALID_WIDTH;
	uint32_t dirty : DIRTY_WIDTH;
	uint32_t tag   : TAG_WIDTH;
	uint8_t  blocks[64]; 
}Cache2;

typedef union {
	struct {
		uint32_t offset : OFF_WIDTH;
		uint32_t grp : GRP_WIDTH;
		uint32_t tag : TAG_WIDTH;
	};
	uint32_t addr;
} cache_addr;

#define NR_GRP (1 << GRP_WIDTH)
#define NR_LINE 16
#define HW_MEM_SIZE (1<<27)

Cache2 L2[NR_GRP][NR_LINE];

void init_l2() {
	int i, j;
	for(i=0;i<NR_GRP;i++){
		for(j=0;j<NR_LINE;j++){
			L2[i][j].valid = 0;
			L2[i][j].dirty = 0;
		}
	}
}

static void l2_write(hwaddr_t addr, void *data, uint8_t *mask){
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);
	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t offset = temp.offset;
	uint32_t grp = temp.grp;
	uint32_t tag = temp.tag;

	int i;
	for(i=0;i<NR_LINE;i++){
		if(L2[grp][i].valid && L2[grp][i].tag == tag){
			
			/* cache hit */
			memcpy_with_mask(L2[grp][i].blocks+offset,data,BURST_LEN,mask);
			L2[grp][i].dirty = 1;
			return ;
		}
	}

	/* cache donot hit */
	srand((unsigned)time(NULL));
	int vic = rand()%16;

	/* write back */
	if(L2[grp][vic].dirty){
		uint32_t pre_addr = (L2[grp][vic].tag << (GRP_WIDTH + OFF_WIDTH)) + (grp << OFF_WIDTH);
		cache_dram(pre_addr,L2[grp][vic].blocks);		
	}

	dram_cache(addr, L2[grp][vic].blocks);
	L2[grp][vic].valid = 1;
	memcpy_with_mask(L2[grp][vic].blocks+offset,data,BURST_LEN,mask);
	L2[grp][vic].dirty = 1;
	L2[grp][vic].tag = tag;
}

void L2_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];
	uint8_t mask[2 * BURST_LEN];
	memset(mask, 0, 2 * BURST_LEN);
	
	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);
	
	l2_write(addr, temp, mask);

	if(offset + len > BURST_LEN){
		/* in different cache block*/
		l2_write(addr+BURST_LEN, temp+BURST_LEN, mask+BURST_LEN);
	}
}

void L2_L1(hwaddr_t addr, void * data) {
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);
	
	cache_addr temp;
	temp.addr = addr & ~BLOCK_MASK;
	uint32_t offset = temp.offset;
	uint32_t grp = temp.grp;
	uint32_t tag = temp.tag;

	int i;
	for(i=0;i<NR_LINE;i++){
		if(L2[grp][i].valid && L2[grp][i].tag == tag){

			/* cache hit */
			memcpy(data,L2[grp][i].blocks+offset,BLOCK_LEN);
			//printf("not in L1, but in L2!\n");
			return ;
		}
	}

	/* cache donot hit */
	srand((unsigned)time(NULL));
	int vic = rand()%8;

	/* write back */
	if(L2[grp][vic].dirty){
		
		uint32_t pre_addr = (L2[grp][vic].tag << (GRP_WIDTH + OFF_WIDTH)) + (grp << OFF_WIDTH);
		cache_dram(pre_addr,L2[grp][vic].blocks);		
	}

	dram_cache(addr, L2[grp][vic].blocks);
	L2[grp][vic].valid = 1;
	L2[grp][vic].dirty = 0;
	L2[grp][vic].tag = tag;
	memcpy(data,L2[grp][vic].blocks+offset,BLOCK_LEN);
}
