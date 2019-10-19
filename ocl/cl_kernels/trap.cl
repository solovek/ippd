kernel void trap (
  global float* y, /* f(x) */
  global float* x,
  global float* area,
  const unsigned int sz)
{
  int i = get_global_id(0);
  if (i < sz - 1)
    /* (b-a)/2 * (f(a) + f(b)) */
    area[i] =
      (x[i+1] - x[i]) / 2 *
      (y[i]   + y[i+1]);
}
