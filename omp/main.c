#include <stdio.h>
#include <stdlib.h>

int* rndarr (int);
void prnarr (int*, int);

int* msort (int*, int);

int main (int argc, char** argv)
{
  int* v;
  int  sz;

  if (argc == 2) {
    sz = atoi(argv[1]);
    v  = rndarr(sz);

    v = msort(v, sz);

    prnarr(v, sz);
  } else {
    printf("usage: %s size\n", argv[0]);
  }

  return 0;
}
