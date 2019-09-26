#include <stdio.h>
#include <stdlib.h>

int* rndarr (int sz)
{
  int* v;
  int  i;

  v = malloc(sz * sizeof(int));

  for (i = 0; i < sz; i++)
    v[i] = rand() % 100;

  return v;
}

void prnarr (int* v, int sz)
{
  int i;

  for (i = 0; i < sz; i++)
    printf("%d ", v[i]);
  printf("\n");
}
