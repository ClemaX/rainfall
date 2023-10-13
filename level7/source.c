#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char	c[80];

int		main(int ac,char **av)
{
  void **A;
  void **B;
  FILE *pass_file;
  
  A = malloc(8);

  A[0] = (void *)1;
  A[1] = malloc(8);

  B = malloc(8);

  B[0] = (void *)2;
  B[1] = malloc(8);

  strcpy((char *)A[1],av[1]);
  strcpy((char *)B[1],av[2]);

  pass_file = fopen("/home/user/level8/.pass","r");

  fgets(c, 68, pass_file);

  puts("~~");

  return 0;
}
