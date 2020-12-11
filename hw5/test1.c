#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3, *p4 ;

	print_sm_containers() ;

	p1 = smalloc(2000) ; 
	printf("smalloc(2000):%p\n", p1) ; 
	print_sm_containers() ;

	p2 = smalloc(2500) ; 
	printf("smalloc(2500):%p\n", p2) ; 
	print_sm_containers() ;

	srealloc(p1, 1000);
	printf("srealloc(1000):%p\n", p1) ; 
	print_sm_containers() ;

	sfree(p2) ; 
	printf("sfree(%p)\n", p2) ; 
	print_sm_containers() ;

	sshrink();
	printf("sshrink()\n");
	print_sm_containers();

	p4 = smalloc(1000) ; 
	printf("smalloc(1000):%p\n", p4) ; 
	print_sm_containers() ;

	print_mem_uses();
}
