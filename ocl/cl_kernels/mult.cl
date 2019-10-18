__kernel void vmult (
  __global float* a,
  __global float* b,
  __global float* c,
  __const unsigned int sz)
{
  int i = get_global_id(0);
  if (i < sz)
    c[i] = a[i] * b[i];
}
