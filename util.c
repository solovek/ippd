#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* rndarr (int sz)
{
  int* v;
  int  i;

  srand(time(NULL));

  v = malloc(sz * sizeof(int));

  for (i = 0; i < sz; i++)
    v[i] = rand() % 100;

  return v;
}

float* rndarrf (int sz)
{
  float* v;
  int    i;

  v = malloc(sz * sizeof(float));

  for (i = 0; i < sz; i++)
    v[i] = rand();

  return v;
}

void prnarr (int* v, int sz)
{
  int i;

  for (i = 0; i < sz; i++)
    printf("%d ", v[i]);
  printf("\n");
}
