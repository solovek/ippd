#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int* rndarr (int);
int* msort  (int*, int);
void prnarr (int*, int);

int g_comm_sz;
int grank;

int main (int argc, char** argv)
{
  int* v = NULL;
  int* r = NULL;
  int  sz;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &g_comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &grank);

  if (argc == 2) {
    sz = atoi(argv[1]);
    
    if (!grank) { /* top */
      v = rndarr(sz);
    }

    v = msort(v, sz);

    if (!grank) /* top again */
      prnarr(v, sz);
  } else {
    printf("usage: %s size\n", argv[0]);
  }

  free(v);
  MPI_Finalize();
  return 0;
}
