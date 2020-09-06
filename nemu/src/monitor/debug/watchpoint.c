#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	WP *wp = free_;
	assert(wp != NULL);
	free_ = free_->next;
	wp->next = NULL;
	if(head == NULL) {
		head = wp;
		wp->NO = 0;
		assert(head!=NULL);
	}else {
		int cnt=1; WP *pos = head;
		while(pos->next != NULL) {
			pos = pos->next;
			cnt ++;
		}
		wp->NO = cnt;
		pos->next = wp;
	}
	assert(head!=NULL);
	return wp;
}

void free_wp(WP *wp) {
	assert(wp!=NULL);
	WP *pos = head;
	if(pos == wp) {
		head = head->next;
		pos = pos->next;
		wp->next = free_;
		free_ = wp;
		return ;
	}else {
		while(pos->next != NULL) {
			if(pos->next == wp){
				pos->next = wp->next;
				pos = pos->next;
				wp->next = free_;
				free_ = wp;
				break;
			}
			pos = pos->next;
		}
	}
	while(pos != NULL) {
		pos->NO --;
		pos = pos->next;
	}
	return ;
}

void info_wp() {
	WP* wp = head;
	while(wp != NULL){
		printf("WatchPoint%d : expression: %s\n",wp->NO,wp->exp);
		wp = wp->next;
	}
}

WP *cal_wp(int no) {
	WP* wp = head;
	while(wp!=NULL && wp->NO<no){
		wp = wp->next;
	}
	return wp;
}
