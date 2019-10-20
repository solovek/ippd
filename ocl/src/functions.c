static float linear (float x)
{
  return x;
}

float(* flookup(char* name)) (float)
{
  return linear;
}
