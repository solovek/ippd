kernel void trap (
  global float* y, /* f(x) */
  global float* x,
  global float* area,
  const unsigned int sz)
{
  int i = get_global_id(0);
  if (i < sz)
    /* (b-a)/2n * (f(a) + f(b)) */
    area[i] =
      (x[i+1] - x[i]) / (2 * sz) *
      (y[i]   + y[i+1]);
}
