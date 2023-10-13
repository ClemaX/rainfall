#include <stdlib.h>
#include <stdio.h>

int32_t	m = 0;

void	v(void)
{
	char str[520];
	
	fgets(str, 0x200, stdin);
	printf(str);

	if (m == 0x40) {
		fwrite("Wait what?!\n", 1, 0xc, stdout);
		system("/bin/sh");
	}
}



int		main(void)
{
	v();
}
