<span style="display: flex; justify-content: space-between;"><span style="text-align: left; display: block;">
</span>
<span style="text-align: center; display: block;">
	[Home](../README.md)
</span>
<span style="text-align: right; display: block;">
	[level1 →](../level1/solution.md)
</span>
</span>

level0
======


Identification
--------------

To identify the file we can use `file`:
```sh
file level0
```

```
level0: setuid ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), statically linked, for GNU/Linux 2.6.24, BuildID[sha1]=0x85cf4024dbe79c7ccf4f30e7c601a356ce04f412, not stripped
```

The file seems to be a 32-bit ELF executable, with the `setuid` bit set.

If we check the file owner we can see which user's privileged will be used when running the executable:
```sh
ls -l level0
```

```
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```

As we can see, the executable is owned by the user `level1` and will be run as this user due to the `setuid` bit.

Dissassembly
------------

To dissassemble the executable we can use `gdb`.
```sh
FUNCTION=main EXECUTABLE=./level0
gdb -batch -ex "set disassembly-flavor intel" -ex "disassemble/r $FUNCTION" "$EXECUTABLE"
```

In the first few lines of output we can see a call to `atoi`, followed by a comparison to `0x1a7`.
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

Later on there is a call to `execv`, which is a function used to run an executable. There are also some function calls to set the real effective ids for the process to the effective ids, which corresponds to the file owner and group's ids due to the `setuid` bit.

```
...
   0x08048ee7 <+39>:	e8 04 7d 00 00	call   0x8050bf0 <strdup>
...
   0x08048f01 <+65>:	e8 6a b7 00 00	call   0x8054670 <geteuid>
...
   0x08048f21 <+97>:	e8 da b7 00 00	call   0x8054700 <setresgid>
...
   0x08048f3d <+125>:	e8 4e b7 00 00	call   0x8054690 <setresuid>
...
   0x08048f51 <+145>:	e8 ea b6 00 00	call   0x8054640 <execv>
```

The call is skipped by the `jne` instruction above, if the result of `atoi` is not `0x1a7`. On the stack, we can see that the argument passed to `atoi` is the first argument given when running the executable.

The argument array's pointer is located at `ebp + 0xc` and the offset `0x4` corresponds to the second string pointer, which is the first argument.

```
...
   0x08048ec9 <+9>:  8b 45 0c       mov     eax,DWORD PTR [ebp+0xc]
   0x08048ecc <+12>: 83 c0 04       add     eax,0x4
   0x08048ecf <+15>: 8b 00          mov     eax,DWORD PTR [eax]
   0x08048ed1 <+17>: 89 04 24       mov     DWORD PTR [esp],eax
   0x08048ed4 <+20>: e8 37 08 00 00 call    0x8049710 <atoi>
   0x08048ed9 <+25>: 3d a7 01 00 00 cmp     eax,0x1a7
   0x08048ede <+30>: 75 78          jne     0x8048f58 <main+152>
...
```

This means that we can reach the `execv` call if we provide the correct value to `atoi` in the executable's first argument.

We do not now yet which executable will be called but we can try to run it anyway to see what it does.

Otherwise we could also use a debugger or disassembler to figure out which arguments are given to `execv` and which command will be run with the elevated privilages.

Exploitation
------------

The decimal value for `0x1a7` is `423`. Hence, passing this string as an argument to the executable results in the execution of a shell, with the `level1` user's user-id and group-id.
```sh
./level0 423
$ whoami
level1
```
