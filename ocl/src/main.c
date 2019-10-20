#include <assert.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void   prnarrf  (float*, int);
float* rndarrf  (int);
char*  readfile (char*);
char** loadpath (char*, int*);

float(* flookup(char*)) (float);

cl_int clCall3 (char*, float*, float*, float*, int);

const char* const errstr_usage =
  "usage: %s [vmult/integral] [function] [sample] [start] [end]\n";

int gsz = 5;

cl_device_id     gdevice;
cl_context       gcontext;
cl_command_queue gcommands;
cl_program       gprogram;

int main (int argc, char** argv)
{
  cl_int err;
  int    i;
  float* h_a;
  float* h_b;
  float* h_c;
  int    length;
  char** kernel_src;
  int    nofiles;

  cl_mem d_a;
  cl_mem d_b;
  cl_mem d_c;

  cl_uint       noplatforms;
  cl_platform_id* platform;

  if (argc < 2) {
    printf(errstr_usage, argv[0]);
    return 0;
  }
  
  err = clGetPlatformIDs(0, NULL, &noplatforms);
  
  platform = calloc(noplatforms, sizeof(cl_platform_id));
  
  for (i = 0; i < noplatforms; i++)
    if ((err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_DEFAULT,
			      1, &gdevice, NULL)) == CL_SUCCESS)
      break;

  /* add check for device_id here */

  gcontext = clCreateContext(0, 1, &gdevice, NULL, NULL, &err);

  gcommands = clCreateCommandQueueWithProperties(gcontext, gdevice,
						 0,        &err);

  kernel_src = loadpath("./cl_kernels/", &nofiles);

  gprogram = clCreateProgramWithSource(gcontext,   nofiles,
				       kernel_src, NULL,
				       &err);
  err = clBuildProgram(gprogram, 0, NULL, NULL, NULL, NULL);

  if (err == CL_BUILD_PROGRAM_FAILURE) {
    char str[1024];
    clGetProgramBuildInfo(gprogram,             gdevice,
			  CL_PROGRAM_BUILD_LOG, 1024,
			  str,                  NULL);
    printf("%s\n", str);
  }

  if (!strcmp(argv[1], "vmult")) { /* exercises 1 and 2 */
    srand(time(NULL));

    h_a = rndarrf(gsz);
    h_b = rndarrf(gsz);
    h_c = rndarrf(gsz);

    length = gsz * sizeof(*h_a);
    
    clCall3("vmult", h_a, h_b, h_c, length);
    
    prnarrf(h_a, gsz);
    prnarrf(h_b, gsz);
    prnarrf(h_c, gsz);
  } else if (!strcmp(argv[1], "integral")) { /* exercise 3 */
    float(* f)(float); /* integration target */
    float   x;
    float   step;
    float   acc;
    
    if (argc < 6) {
      printf(errstr_usage, argv[0]);
      goto argv_err;
    }

    f = flookup(argv[2]);
    step = atof(argv[3]);
    gsz = (atof(argv[5]) - atof(argv[4])) / step + 1;

    length = gsz * sizeof(*h_a);

    h_a = malloc(length);
    h_b = malloc(length);
    h_c = malloc(length);
    
    for (i = 0, x = 0; i < gsz; i++, x += step) {
      h_a[i] = f(x);
      h_b[i] = x;
    }

    clCall3("trap", h_a, h_b, h_c, length);
    
    for (i = 0, acc = 0; i < gsz-1; i++) {
      acc += h_c[i];
    }

    printf("area: %f\n", acc);
  }

  free(h_a);
  free(h_b);
  free(h_c);
  
 argv_err:
  clReleaseProgram(gprogram);
  clReleaseCommandQueue(gcommands);
  clReleaseContext(gcontext);

  for (i = 0; i < nofiles; i++)
    free(kernel_src[i]);
  free(kernel_src);
  
  return 0;
}

cl_int clCall3 (char*  name,
		float* h_a,
		float* h_b,
		float* h_c,
		int    len)
{
  cl_int err;
  cl_mem d_a;
  cl_mem d_b;
  cl_mem d_c;
  size_t global;
  cl_kernel kernel;
  
  kernel = clCreateKernel(gprogram, name, &err);
  
  d_a = clCreateBuffer(gcontext, CL_MEM_READ_ONLY,
		       len,      NULL, &err);
  d_b = clCreateBuffer(gcontext, CL_MEM_READ_ONLY,
		       len,      NULL, &err);
  d_c = clCreateBuffer(gcontext, CL_MEM_WRITE_ONLY,
		       len,      NULL, &err);
  
  err = clEnqueueWriteBuffer(gcommands, d_a, CL_TRUE,    0,
			     len,       h_a,       0, NULL,
			     NULL);
  err = clEnqueueWriteBuffer(gcommands, d_b, CL_TRUE,    0,
			     len,       h_b,       0, NULL,
			     NULL);

  err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
  err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &gsz);

  global = gsz;
  err = clEnqueueNDRangeKernel(gcommands, kernel,  1,
			       NULL,      &global, NULL,
			       0,         NULL,    NULL);

  err = clFinish(gcommands);

  err = clEnqueueReadBuffer(gcommands, d_c, CL_TRUE, 0,
			    len,       h_c, 0,       NULL,
			    NULL);

  clReleaseMemObject(d_a);
  clReleaseMemObject(d_b);
  clReleaseMemObject(d_c);
  clReleaseKernel(kernel);
  
  return 0;
}
