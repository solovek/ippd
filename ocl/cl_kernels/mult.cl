kernel void vmult (
  global float* a,
  global float* b,
  global float* c,
  const unsigned int sz)
{
  int i = get_global_id(0);
  if (i < sz)
    c[i] = a[i] * b[i];
}
