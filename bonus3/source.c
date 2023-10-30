#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int ac, char **av)
{
	int err;
	char buff[64 + 1 + 66];
	FILE *pass_file;

	pass_file = fopen("/home/user/end/.pass","r");

	bzero(buff, 33);

	if ((pass_file == NULL || (ac != 2)))
		err = -1;
	else
	{
		fread(buff, 1, 66, pass_file);
		buff[64] = '\0';

		buff[atoi(av[1])] = '\0';

		fread(buff + 64 + 1, 1, 65, pass_file);

		fclose(pass_file);

		if (strcmp(buff, av[1]) == 0)
			execl("/bin/sh", "sh", NULL);
		else
			puts(buff + 64 + 1);

		err = 0;
	}
	return err;
}
