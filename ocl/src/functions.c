#include <math.h>
#include <stdlib.h>
#include <string.h>

struct fntable {
  char* name;
  double(* fn) (double);
};

extern const struct fntable fntable[];

double(* flookup(char* name)) (double)
{
  int i;
  for (i = 0; fntable[i].name; i++) {
    if (!strcmp(name, fntable[i].name))
      return fntable[i].fn;
  }

  return NULL;
}

static double linear (double x) {return x;}
static double square (double x) {return x * x;}

const struct fntable fntable[] = {
  {"linear", linear}, {"square", square}, {"sqrt", sqrt},
  {NULL, NULL}
};
  
