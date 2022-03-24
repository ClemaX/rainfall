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
