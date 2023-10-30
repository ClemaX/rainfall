#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PROMPT " - "

void	p(char *dest, char *prompt)
{
	char *end;
	char buffer[4104];
	
	puts(prompt);

	read(0, buffer, 4096);

	end = strchr(buffer, 10);

	*end = '\0';

	strncpy(dest, buffer, 20);
}



void	pp(char *str)
{
	size_t	length;
	char	buff_a[20];
	char	buff_b[20];

	p(buff_a, PROMPT);
	p(buff_b, PROMPT);

	strcpy(str, buff_a);

	length = strlen(str);

	str[length] = ' ';
	strcat(str, buff_b);
}



int		main()
{
	char	str[54];
	
	pp(str);
	puts(str);

	return 0;
}
