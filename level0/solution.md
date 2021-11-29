# level0

## Steps

### Dissassembly

To dissassemble the executable we can use gdb.
```sh
FUNCTION=main EXECUTABLE=./level0
gdb -batch -ex "set disassembly-flavor intel" -ex "disassemble/r $FUNCTION" "$EXECUTABLE"
```

In the first few lines of output we can see a call to atoi, followed by a comparison to 0x1a7.
```
Dump of assembler code for function main:
   0x08048ec0 <+0>:  55             push    ebp
   0x08048ec1 <+1>:  89 e5          mov     ebp,esp
   0x08048ec3 <+3>:  83 e4 f0       and     esp,0xfffffff0
   0x08048ec6 <+6>:  83 ec 20       sub     esp,0x20
   0x08048ec9 <+9>:  8b 45 0c       mov     eax,DWORD PTR [ebp+0xc]
   0x08048ecc <+12>: 83 c0 04       add     eax,0x4
   0x08048ecf <+15>: 8b 00          mov     eax,DWORD PTR [eax]
   0x08048ed1 <+17>: 89 04 24       mov     DWORD PTR [esp],eax
   0x08048ed4 <+20>: e8 37 08 00 00 call    0x8049710 <atoi>
   0x08048ed9 <+25>: 3d a7 01 00 00 cmp     eax,0x1a7
   0x08048ede <+30>: 75 78          jne     0x8048f58 <main+152>
...
```

The decimal value for `0x1a7` is `423`. Hence, passing this string as an argument to the executable results in the execution of a shell, with the `level1` user's user-id and group-id.
```sh
./level0 423
$ whoami
level1
```
