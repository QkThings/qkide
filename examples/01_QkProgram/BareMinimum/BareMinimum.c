#include <qk_program.h>
#include "BareMinimum.h"

/*
	This is the most minimal program you can compile.
	It is so minimal it does nothing but to initialize the hardware and
	to prepare it for the program you will write.
*/

void qk_setup()
{
	/*
	You MUST allways provide this function.
	Everything that must be initialized or run once at the beginning
	of your program must be placed here. You may check other
	more complete examples to get an idea of what to write here.
	*/
}

int main()
{
	/* 
	Every C program needs a "main" function and this is no exception.
	The special thing here is that qkprogram also provides you the 
	main loop of you program which is called inside the qk_main() function,
	called bellow.
	*/
	return qk_main();
}

