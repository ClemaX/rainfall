#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

uint32_t	m = 0;

void	p(char *str)
{
	printf(str);
}

void n(void)
{
	char str[520];
	
	fgets(str, 512, stdin);

	p(str);

	if (m == 0x1025544)
		system("/bin/cat /home/user/level5/.pass");
}

int	main(void)
{
	n();
}
