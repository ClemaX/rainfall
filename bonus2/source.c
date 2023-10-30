#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define LANG_FI "fi"
#define LANG_NL "nl"

#define S_GREETING_EN "Hello "
#define S_GREETING_FI "Hyv\xC3\xA4\xC3\xA4 p\xC3\xA4iv\xC3\xA4\xC3\xA4 "
#define S_GREETING_NL "Goedemiddag! "

enum lang_id
{
		LANG_ID_EN,
		LANG_ID_FI,
		LANG_ID_NL,
};

static enum lang_id		language;

int greetuser(char *message)
{
	char greeting[72];

	if (language == LANG_ID_FI)
		strcpy(greeting, S_GREETING_FI);
	else if (language == LANG_ID_NL)
		strcpy(greeting, S_GREETING_NL);
	else if (language == LANG_ID_EN)
		strcpy(greeting, S_GREETING_EN);

	strcat(greeting, message);

	return puts(greeting);
}


int main(int ac, char **av)
{
	int ret;
	char message[76];
	char *lang;

	if (ac == 3)
	{
		bzero(message, sizeof(message));

		strncpy(message, av[1], 40);
		strncpy(message + 40, av[2], 32);

		lang = getenv("LANG");

		if (lang != NULL)
		{
			ret = memcmp(lang, LANG_FI, sizeof(LANG_FI) - 1);

			if (ret == 0)
				language = LANG_ID_FI;
			else
			{
				ret = memcmp(lang, LANG_NL, sizeof(LANG_NL) - 1);

				if (ret == 0)
					language = LANG_ID_NL;
			}
		}
		ret = greetuser(message);
	}
	else
		ret = 1;
	return ret;
}
