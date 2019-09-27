#include <assert.h>
#include <omp.h>
#include <stdlib.h>

static int* split (int*, int);
static int* merge (int*, int*, int, int);

int* msort (int* v, int sz)
{
  int* r;
  
# pragma omp parallel
# pragma omp single 
  r = split(v, sz);

  return r;
}

#define GETFLG(X) (X == 2? 0 : (X == 3? 2 : 3))

static int* split (int* v, int sz)
{
  int* u = NULL;
  int* w = NULL;
  int  usz;
  int  wsz;
  int  flg;

  assert(v);

  if (sz > 1) {
    flg = GETFLG(sz);
    
    usz = sz / 2;
    wsz = sz - usz;

#   pragma omp task shared(u)
    u = split(v,       usz);
    w = split(v + usz, wsz);

#   pragma omp taskwait
    v = merge(u, w, usz, wsz);

    if (flg & 0x01 && u)
      free(u);
    if (flg & 0x02 && w)
      free(w);
  }

  return v;
}

#undef GETFLG

static int* merge (int* v, int* w, int vsz, int wsz)
{
  int* r;
  int  rsz;
  int  i, j, k;

  rsz = vsz + wsz;
  r = malloc(rsz * sizeof(int));

  i = 0;
  j = 0;
  k = 0;
  
  while (i < vsz && j < wsz) {
    if (v[i] <= w[j]) {
      r[k] = v[i];
      i++;
    } else {
      r[k] = w[j];
      j++;
    }
    k++;
  }

  for (; i < vsz; i++, k++)
    r[k] = v[i];
  for (; j < wsz; j++, k++)
    r[k] = w[j];

  return r;
}
