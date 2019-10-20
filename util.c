#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* generators */
int* rndarr (int sz)
{
  int* v;
  int  i;

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
    v[i] = (float)rand() / 10000000;

  return v;
}

/* IO */
void prnarr (int* v, int sz)
{
  int i;

  for (i = 0; i < sz; i++)
    printf("%d ", v[i]);
  printf("\n");
}

void prnarrf (float* v, int sz)
{
  int i;

  for (i = 0; i < sz; i++)
    printf("%.2f ", v[i]);
  printf("\n");
}

char* readfile (char* path)
{
  struct stat stbuf;
  FILE*       in;
  char*       str;
  int         len;

  str   = NULL;
  errno = 0;

  if (stat(path, &stbuf)) {
    perror(NULL);
    goto file_err;
  }
  
  if (!(in = fopen(path, "r"))) {
    perror(NULL);
    goto file_err;
  }
  
  len = stbuf.st_size;
  if (!(str = calloc(len, sizeof(*str)))) {
    perror(NULL);
    goto alloc_err;
  }

  if (fread(str, sizeof(*str), len, in) < len) {
    perror(NULL);
    free(str);
    str = NULL;
  }

 alloc_err:
  fclose(in);
 file_err:
  return str;
}

char** loadpath (char* path, int* len)
{
  DIR*           dir;
  struct dirent* entry;
  char**         file;
  
  file = NULL;
  *len = 0;
  
  if (!(dir = opendir(path))) {
    perror(NULL);
    goto dir_err;
  }

  /* i think 512 is a pretty reasonable number of files */
  file = calloc(512, sizeof(*file));
  
  while ((entry = readdir(dir))) {
    char* fullpath;
    if (!strcmp(entry->d_name, "." ) ||
	!strcmp(entry->d_name, "..") ||
	entry->d_type == DT_DIR)
      continue;

    fullpath = calloc(strlen(path) + strlen(entry->d_name),
		      sizeof(*fullpath));
    fullpath = strcat(strcat(fullpath,
			     path),
		      entry->d_name);
    
    file[(*len)++] = readfile(fullpath);

    free(fullpath);
  }

  file = realloc(file, (*len) * sizeof(*file));

  closedir(dir);
 dir_err:
  return file;
}

#ifdef __OPENCL_VERSION__
#include <CL/cl.h>

/* imo this is stupid find a better way to abstract it */
cl_kernel load_kernel (char*       path,
		       cl_program* program,
		       cl_context  context,
		       cl_int*     err)
{
  char*     src;
  cl_kernel ko;

  ko = NULL;
  
  if ((src = readfile(path))) {
    /* add error checking later */
    *prog = clCreateProgramWithSource(context, 1, &src, NULL, err);
    *err  = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    ko    = clCreateKernel(program, "path", &err);
  }
  
  return ko;
}

#endif
