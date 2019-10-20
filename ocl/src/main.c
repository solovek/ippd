#include <assert.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern void   prnarrf  (float*, int);
extern float* rndarrf  (int);
extern char*  readfile (char*);
extern char** loadpath (char*, int*);

extern float(* flookup(char*)) (float);

int gsz = 5;

float foo (float);

const char* const errstr_usage =
  "usage: %s [vmult/integral] [function] [sample] [start] [end]\n";

int main (int argc, char** argv)
{
  cl_int err;
  int    i;
  float* h_a;
  float* h_b;
  float* h_c;
  int    length;
  size_t global;
  char** kernel_src;
  int    nofiles;

  cl_device_id     device_id;
  cl_context       context;
  cl_command_queue commands;
  cl_program       program;
  cl_kernel        kernel;

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
			      1, &device_id, NULL)) == CL_SUCCESS)
      break;

  /* add check for device_id here */

  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

  commands = clCreateCommandQueueWithProperties(context, device_id,
						      0, &err);

  kernel_src = loadpath("./cl_kernels/", &nofiles);

  program = clCreateProgramWithSource(context,    nofiles,
				      kernel_src, NULL,
				      &err);
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

  if (err == CL_BUILD_PROGRAM_FAILURE) {
    char str[1024];
    clGetProgramBuildInfo(program,              device_id,
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
    kernel = clCreateKernel(program, "vmult", &err);
  } else if (!strcmp(argv[1], "integral")) { /* exercise 3 */
    float(* f)(float); /* integration target */
    float   x;
    float   step;
    
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

    kernel = clCreateKernel(program, "trap", &err);
  }

  d_a = clCreateBuffer(context, CL_MEM_READ_ONLY,
		       length,  NULL, &err);
  d_b = clCreateBuffer(context, CL_MEM_READ_ONLY,
		       length,  NULL, &err);
  d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		       length,  NULL, &err);
  
  err = clEnqueueWriteBuffer(commands, d_a, CL_TRUE,    0,
			     length,   h_a,       0, NULL,
			     NULL);
  err = clEnqueueWriteBuffer(commands, d_b, CL_TRUE,    0,
			     length,   h_b,       0, NULL,
			     NULL);

  err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
  err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &gsz);

  global = gsz;
  err = clEnqueueNDRangeKernel(commands, kernel,  1,
			       NULL,     &global, NULL,
			       0,        NULL,    NULL);

  err = clFinish(commands);

  err = clEnqueueReadBuffer(commands, d_c, CL_TRUE, 0,
			    length,   h_c, 0,       NULL,
			    NULL);
  if (!strcmp(argv[1], "vmult")) {
    prnarrf(h_a, gsz);
    prnarrf(h_b, gsz);
    prnarrf(h_c, gsz);
  } else if (!strcmp(argv[1], "integral")) {
    float acc;
    for (i = 0, acc = 0; i < gsz-1; i++) {
      acc += h_c[i];
    }

    printf("area: %f\n", acc);
  }

  free(h_a);
  free(h_b);
  free(h_c);
  
  clReleaseMemObject(d_a);
  clReleaseMemObject(d_b);
  clReleaseMemObject(d_c);
  clReleaseKernel(kernel);
 argv_err:
  clReleaseProgram(program);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  for (i = 0; i < nofiles; i++)
    free(kernel_src[i]);
  free(kernel_src);
  
  return 0;
}

float foo (float x)
{
  return 10;
}
