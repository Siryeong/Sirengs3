#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3, *p4, *p5, *p6;

	print_sm_containers() ;

	p1 = smalloc(2000) ; 
	printf("smalloc(2000):%p\n", p1) ; 
	print_sm_containers() ;

	p2 = smalloc(2500) ; 
	printf("smalloc(2500):%p\n", p2) ; 
	print_sm_containers() ;

	p3 = smalloc(3000) ; 
	printf("smalloc(3000):%p\n", p3) ; 
	print_sm_containers() ;
	
	p4 = smalloc(500) ; 
	printf("smalloc(500):%p\n", p4) ; 
	print_sm_containers() ;

	p5 = smalloc(300) ; 
	printf("smalloc(300):%p\n", p5) ; 
	print_sm_containers() ;
	
	p6 = smalloc(100) ; 
	printf("smalloc(100):%p\n", p6) ; 
	print_sm_containers() ;

	sfree(p5) ; 
	printf("sfree(%p)\n", p5) ; 
	print_sm_containers() ;

	sfree(p4) ; 
	printf("sfree(%p)\n", p4) ; 
	print_sm_containers() ;
	
	sfree(p6) ; 
	printf("sfree(%p)\n", p6) ; 
	print_sm_containers() ;
}
