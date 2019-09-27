#include <stdio.h>
#include <stdlib.h>

int* rndarr (int);
void prnarr (int*, int);

int* msort (int*, int);

int main (int argc, char** argv)
{
  int* v;
  int* w;
  int  sz;

  if (argc == 2) {
    sz = atoi(argv[1]);
    v  = rndarr(sz);

    w = msort(v, sz);

    prnarr(w, sz);

    free(v);
    free(w);
  } else {
    printf("usage: %s size\n", argv[0]);
  }

  return 0;
}
