#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "smalloc.h" 

sm_container_t sm_head = {
	0,
	&sm_head, 
	&sm_head,
	0 
} ;

static 
void * 
_data (sm_container_ptr e)
{
	return ((void *) e) + sizeof(sm_container_t) ;
}

static 
void 
sm_container_split (sm_container_ptr hole, size_t size)
{
	sm_container_ptr remainder = (sm_container_ptr) (_data(hole) + size) ;

	remainder->dsize = hole->dsize - size - sizeof(sm_container_t) ;	
	remainder->status = Unused ;
	remainder->next = hole->next ;
	remainder->prev = hole ;
	hole->dsize = size ;
	hole->next->prev = remainder ;
	hole->next = remainder ;
}

static
void
sm_container_merge (sm_container_ptr hole)
{
	sm_container_ptr empty = 0x0, itr = 0x0;

	if(hole->status == Busy) return;
	
	for(itr = hole->next; (itr->status == Unused) && (itr != &sm_head); itr = itr->next){
		hole->dsize += (itr->dsize + sizeof(sm_container_t));
		hole->next = itr->next;
		hole->next->prev = hole;
	}
	for(itr = hole->prev; (itr->status == Unused) && (itr != &sm_head); itr = itr->prev){
		itr->dsize += (hole->dsize + sizeof(sm_container_t));
		itr->next = hole->next;
		itr->next->prev = itr;
	}

}

static 
void * 
retain_more_memory (int size)
{
	sm_container_ptr hole ;
	int pagesize = getpagesize() ;
	int n_pages = 0 ;

	n_pages = (sizeof(sm_container_t) + size + sizeof(sm_container_t)) / pagesize  + 1 ;
	hole = (sm_container_ptr) sbrk(n_pages * pagesize) ;
	if (hole == 0x0)
		return 0x0 ;
	
	hole->status = Unused ;
	hole->dsize = n_pages * getpagesize() - sizeof(sm_container_t) ;
	return hole ;
}

void * 
smalloc (size_t size) 
{
	sm_container_ptr hole = 0x0, itr = 0x0 ;

	for (itr = sm_head.next ; itr != &sm_head ; itr = itr->next) {
		if (itr->status == Busy)
			continue ;
		if ((itr->dsize == size) || (size + sizeof(sm_container_t) < itr->dsize)) {
			if(hole == 0x0) hole = itr;
			if(hole->dsize > itr->dsize) hole = itr ; 
		}
	}
	if (hole == 0x0) {
		hole = retain_more_memory(size) ;
		if (hole == 0x0)
			return 0x0 ;
		hole->next = &sm_head ;
		hole->prev = sm_head.prev ;
		(sm_head.prev)->next = hole ;
		sm_head.prev = hole ;
	}
	if (size < hole->dsize) 
		sm_container_split(hole, size) ;
	hole->status = Busy ;
	return _data(hole) ;
}

void *
srealloc (void * p, size_t newsize)
{
	sm_container_ptr hole = 0x0, itr = 0x0;

	for(itr = sm_head.next ; itr != &sm_head ; itr = itr->next){
		if(p == _data(itr))
			break;	
	}
	if(itr->dsize > newsize){
		sm_container_split(itr, newsize);
		sm_container_merge(itr->next);
	} else if(itr->next->status == Unused && ((itr->dsize + itr->next->dsize) > newsize) || ((itr->dsize + itr->next->dsize + sizeof(sm_container_t)) == newsize)){
		itr->dsize += (itr->next->dsize + sizeof(sm_container_t));
		itr->next = itr->next->next;
		itr->next->next->prev = itr;
		if(itr->dsize > newsize)
			sm_container_split(itr, newsize);
		hole = itr;
	} else {
		hole = (sm_container_ptr)smalloc(newsize);
		memcpy(hole, itr, itr->dsize);
		sfree(itr);
	}
	

	return hole;
}

void 
sfree (void * p)
{
	sm_container_ptr itr ;
	for (itr = sm_head.next ; itr != &sm_head ; itr = itr->next) {
		if (p == _data(itr)) {
			itr->status = Unused ;
			sm_container_merge(itr);
			break ;
		}
	}
}

void
sshrink()
{
	sm_container_ptr itr, tmp;
	for(itr = sm_head.prev ; (itr != &sm_head) && (itr->status == Unused) ; itr = itr->prev ){
		tmp = itr;
		itr->next->prev = itr->prev;
		itr->prev->next = itr->next;
		brk(tmp + 4);
	}
}

void 
print_sm_containers ()
{
	sm_container_ptr itr ;
	int i ;

	printf("==================== sm_containers ====================\n") ;
	for (itr = sm_head.next, i = 0 ; itr != &sm_head ; itr = itr->next, i++) {
		printf("%3d:%p:%s:", i, _data(itr), itr->status == Unused ? "Unused" : "  Busy") ;
		printf("%8d:", (int) itr->dsize) ;

		int j ;
		char * s = (char *) _data(itr) ;
		for (j = 0 ; j < (itr->dsize >= 8 ? 8 : itr->dsize) ; j++) 
			printf("%02x ", s[j]) ;
		printf("\n") ;
	}
	printf("\n") ;

}

void
print_mem_uses()
{
	sm_container_ptr itr;
	int i;
	size_t use = 0, unuse = 0;

	printf("===================== msm_uses ===================\n");
	for(itr = sm_head.next ; itr != &sm_head ; itr = itr->next){
		if(itr->status == Busy) { use += (itr->dsize + sizeof(sm_container_t)); }
		else{ unuse += (itr->dsize + sizeof(sm_container_t)); }
	}
	fprintf(stderr, "retained : %lu, allocated : %lu, not allocated : %lu\n\n", use+unuse, use, unuse);

}


