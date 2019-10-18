#include <assert.h>
#include <CL/cl.h>
#include <stdlib.h>

extern float* rndarrf (int);

const char* vmult =
  "__kernel void vmult ("
  "  __global float* a,"
  "  __global float* b,"
  "  __global float* c,"
  "  __const unsigned int sz)"
  "{"
  "  int i = get_global_id(0);"
  "  if (i < sz)"
  "    c[i] = a[i] * b[i];"
  "}"
  "\n";

const int gsz = 1024;

int main (int argc, char** argv)
{
  int err;
  int i;
  float* h_a;
  float* h_b;
  float* h_c;
  int    length;
  size_t global;

  cl_device_id     device_id;
  cl_context       context;
  cl_command_queue commands;
  cl_program       program;
  cl_kernel        ko_vmult;

  cl_mem d_a;
  cl_mem d_b;
  cl_mem d_c;

  cl_uint       noplatforms;
  cl_platform_id* platform;
  
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

  program = clCreateProgramWithSource(context, 1, &vmult, NULL, &err);

  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

  ko_vmult = clCreateKernel(program, "vmult", &err);

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

  err  = clSetKernelArg(ko_vmult, 0, sizeof(cl_mem), &d_a);
  err |= clSetKernelArg(ko_vmult, 1, sizeof(cl_mem), &d_b);
  err |= clSetKernelArg(ko_vmult, 2, sizeof(cl_mem), &d_c);
  err |= clSetKernelArg(ko_vmult, 3, sizeof(unsigned int), &gsz);

  global = gsz;
  err = clEnqueueNDRangeKernel(commands, ko_vmult,    1,
			           NULL,  &global, NULL,
			              0,     NULL, NULL);

  err = clFinish(commands);

  err = clEnqueueReadBuffer(commands, d_c, CL_TRUE,    0,
			      length, h_c,       0, NULL,
			        NULL);

  clReleaseMemObject(d_a);
  clReleaseMemObject(d_b);
  clReleaseMemObject(d_c);
  clReleaseProgram(program);
  clReleaseKernel(ko_vmult);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  free(h_a);
  free(h_b);
  free(h_c);
  
  return 0;
}
