#include <stdlib.h>
#include <stdio.h>

void	run(void)
{
	fwrite("Good... Wait what?\n", 1, 0x13, stdout);
	system("/bin/sh");
}

int		main()
{
	char str [76];

	gets(str);
}
