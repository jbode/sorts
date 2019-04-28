#include <stdio.h>

#define FOOFUNCDEF(type)      \
  void foo_##(type)( type var ) \
  {                           \
    // code goes here       \
  }

FOOFUNCDEF(int)

FOOFUNCDEF(double)

FOOFUNCDEF(char *)


