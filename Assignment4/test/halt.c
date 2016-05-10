/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */


#include "syscall.h"

int
main()
{
	Create("aaaaaaa");
	Create("abbbbbbas");
	Create("acswqdf");

	Write("Ahmeis chussar",30,Open("aaaaaaa"));  
	Write("avac is chussar",30,Open("abbbbbbas"));
	Write("jaksf is chussar",30,Open("acswqdf"));

	Read("aaaaaaa",30,Open("aaaaaaa"));
//	cout<<"Opend\n";
    /* not reached */
    Exit(1);
}
