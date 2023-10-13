#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef void	(*fun_ptr)();

void n()
{
	system("/bin/cat /home/user/level7/.pass");
}

void m()
{
	puts("Nope");
}

int main(int ac,char **av)
{
	char	*str;
	fun_ptr	fun;
	
	str = malloc(0x40);
	fun = malloc(4);

	fun = m;

	strcpy(str, av[1]);

	fun();
}
