__kernel void trap (
  __global float* y,   /* height */
  __global float* x,   /* width */
  __global float* area,
  __const unsigned int sz)
{
  int i = get_global_id(0);
  if (i < sz) {
    /* 1/2(b-a) * (f(a) + f(b)) */
    //b[i] = (a[i] + a[i + 1]);
  }
}
