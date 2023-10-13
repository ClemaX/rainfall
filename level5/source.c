#include <stdlib.h>
#include <stdio.h>

void	o()
{
	system("/bin/sh");
										// WARNING: Subroutine does not return
	exit(1);
}

void	n()
{
	char s [520];
	
	fgets(s, 512, stdin);
	printf(s);
										// WARNING: Subroutine does not return
	exit(1);
}

int		main()
{
	n();
}
