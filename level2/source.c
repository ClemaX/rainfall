#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void	p(void)
{
	char str[76];

	fflush(stdout);

	gets(str);

	if ((*(uintptr_t*)(str + 76) & 0xb0000000) == 0xb0000000)
	{
		printf("(%p)\n", *(void**)(str + 76));
		exit(1);
	}

	puts(str);

	strdup(str);
}

int		main(void)
{
	p();
}
