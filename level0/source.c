#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int ac,char **av)

{
  int n;
  gid_t egid;
  uid_t euid;
  char *exec_av [2];
  
  n = atoi(av[1]);
  if (n == 423) {
    exec_av[0] = strdup("/bin/sh");
    exec_av[1] = NULL;

    egid = getegid();
    euid = geteuid();

    setresgid(egid, egid, egid);
    setresuid(euid, euid, euid);

    execv("/bin/sh", exec_av);
  }
  else {
    fwrite("No !\n", 1, 5, stderr);
  }
  return 0;
}
