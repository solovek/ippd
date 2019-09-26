#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void split  (int*, int,  int);
static void merge_ (int*, int,  int, int);
static int* merge  (int*, int*, int, int, int);

extern int g_comm_sz;
extern int grank;

int* msort (int* v, int sz)
{
  int psz;
  int fsz;
  int i;
  int* w;

  fsz = 0;
  psz = sz / (g_comm_sz - 1);
  w = malloc(psz * sizeof(int));
  
  if (!grank) { /* top */
    for (i = 1; i < g_comm_sz; i++) { /* scatter the array */
      MPI_Send(v + psz * (i-1), psz, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    
    /* use this to make it non destructive*/
    //v = NULL;
    
    for (i = 1; i < g_comm_sz; i++) { /* last merges */
      /*receive sorted sub arrays here*/
      MPI_Recv(w, psz, MPI_INT, i, 0, MPI_COMM_WORLD,
	                              MPI_STATUS_IGNORE);
      v = merge(v, w, fsz, psz, 1);
      fsz += psz;
    }

    return v;
  } else { /* bottoms */
    MPI_Recv(w, psz, MPI_INT, 0, 0, MPI_COMM_WORLD,
	                            MPI_STATUS_IGNORE);
    split(w, 0, psz);
    MPI_Send(w, psz, MPI_INT, 0, 0, MPI_COMM_WORLD);
    return NULL;
  }
}

static void split (int* v, int l, int r)
{
  int m;
  
  if (l < r) {
    m = (l + r) / 2;
    split(v,   l, m);
    split(v, m+1, r);

    merge_(v, l, m, r);
  }
}

static int* merge (int* v, int* w, int vsz, int wsz, int flg)
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
    } else if (v[i] > w[j]) {
      r[k] = w[j];
      j++;
    }
    k++;
  }

  while (i < vsz)
    r[k++] = v[i++];
  while (j < wsz)
    r[k++] = w[j++];

  if (flg & 0x01)
    free(v);
  if (flg & 0x02)
    free(w);

  return r;
}

static void merge_ (int* v, int l, int m, int r)
{
  int i, j, k; 
  int n1 = m - l + 1; 
  int n2 = r - m; 
  
  /* create temp arrays */
  int* L = malloc(n1 * sizeof(int));
  int* R = malloc(n1 * sizeof(int));
  
  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < n1; i++) 
    L[i] = v[l + i]; 
  for (j = 0; j < n2; j++) 
    R[j] = v[m + 1+ j]; 
  
  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray 
  j = 0; // Initial index of second subarray 
  k = l; // Initial index of merged subarray 

  while (i < n1 && j < n2) { 
    if (L[i] <= R[j]) { 
      v[k] = L[i]; 
      i++; 
    } else { 
      v[k] = R[j]; 
      j++; 
    } 
    k++; 
  } 
  
  /* Copy the remaining elements of L[], if there 
     are any */
  while (i < n1) { 
    v[k] = L[i]; 
    i++; 
    k++; 
  } 
  
  /* Copy the remaining elements of R[], if there 
     are any */
  while (j < n2) { 
    v[k] = R[j]; 
    j++; 
    k++; 
  }

  free(L);
  free(R);
}
