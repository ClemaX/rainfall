#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int ac, char **av)
{
	int		status;
	char	buffer[40];
	int		n;
	
	n = atoi(av[1]);

	if (n < 10) {
		memcpy(buffer, av[2], n * 4);

		if (n == 0x574f4c46) {
			execl("/bin/sh", "sh", NULL);
		}

		status = 0;
	}
	else {
		status = 1;
	}
	return status;
}

