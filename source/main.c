#include <stdio.h>
#include <stdlib.h>
#include "increase.h"

int main(int argc, char *argv[])
{
	long var = 0;
	long incvar = 0;

	if(argc <2)
	{
		printf("haven't give argment, return 1\n");
		return 1;
	}
	var = strtoul(argv[1], NULL, 10);
	incvar = inc_var(var);

	printf("the argcment is %ld, Now is %ld\n", var, incvar);
	return 0;
}

