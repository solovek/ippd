#include <assert.h>
#include <CL/cl.h>
#include <stdlib.h>
#include <time.h>

extern void   prnarrf  (float*, int);
extern float* rndarrf  (int);
extern char*  readfile (char*);
extern char** loadpath (char*, int*);

const int gsz = 5;

int inner_main ();

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
  int  nokernels;

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

  srand(time(NULL));
  
  h_a = rndarrf(gsz);
  h_b = rndarrf(gsz);
  h_c = rndarrf(gsz);

  length = gsz * sizeof(float);
  
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

  kernel_src = loadpath("./cl_kernels/", &nokernels);

  program = clCreateProgramWithSource(context,    nokernels,
				      kernel_src, NULL,
				      &err);

  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  
  kernel = clCreateKernel(program, "vmult", &err);

  if (!err) {
    printf("deu caca\n");
  }
  
  d_a = clCreateBuffer(context, CL_MEM_READ_ONLY,
		        length, NULL, &err);
  d_b = clCreateBuffer(context, CL_MEM_READ_ONLY,
		        length, NULL, &err);
  d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		        length, NULL, &err);

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
  err = clEnqueueNDRangeKernel(commands, kernel,    1,
			           NULL,  &global, NULL,
			              0,     NULL, NULL);

  err = clFinish(commands);

  err = clEnqueueReadBuffer(commands, d_c, CL_TRUE,    0,
			      length, h_c,       0, NULL,
			        NULL);

  prnarrf(h_a, gsz);
  prnarrf(h_b, gsz);
  prnarrf(h_c, gsz);
  
  clReleaseMemObject(d_a);
  clReleaseMemObject(d_b);
  clReleaseMemObject(d_c);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  free(h_a);
  free(h_b);
  free(h_c);
  
  return 0;
}

int inner_main ()
{
  
}
