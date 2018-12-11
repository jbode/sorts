#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

void hpsort1(unsigned int n, double *data)
{
  unsigned int i, ir, j, l;
  double rra;

  if (n < 2) return;
  l = (n - 2) / 2 + 1;
  ir = n - 1;

  for (;;)
    {
      if (l > 0) rra = data[--l];
      else
        {
          rra = data[ir];
          data[ir] = data[0];
          if (--ir == 0) { data[0] = rra; break; }
        }

      i = l;
      j = l + l + 1;
      while (j <= ir)
        {
          if (j < ir && data[j] < data[j+1]) ++j;
          if (rra < data[j])
            {
              data[i] = data[j];
              i = j;
              j += j + 1;
            }
          else break;
        }
      data[i] = rra;
    }

  return;
}

void hpsort2(unsigned int n, double *data)
{
  unsigned int i, ir, j, l;
  double rra;

  ++data;

  if (n < 2) return;
  l = (n - 2) / 2 + 1;
  ir = n - 1;

  for (;;)
    {
      if (l > 0) rra = data[--l];
      else
        {
          rra = data[ir];
          data[ir] = data[0];
          if (--ir == 0) { data[0] = rra; break; }
        }

      i = l;
      j = l + l + 1;
      while (j <= ir)
        {
          if (j < ir && data[j] < data[j+1]) ++j;
          if (rra < data[j])
            {
              data[i] = data[j];
              i = j;
              j += j + 1;
            }
          else break;
        }
      data[i] = rra;
    }

  return;
}

void init( double *arr, size_t size )
{
  for ( size_t i = 0; i < size; i++ )
    arr[i] = (double) rand() / RAND_MAX;
}

int validate( double *arr, size_t size )
{
  size_t i;
  for ( i = 0; i < size-1 && arr[i] <= arr[i+1]; i++ )
    ;
  return i == size-1;
}
 
void dump( FILE *stream, double *arr, size_t size, int width )
{
  int bytesWritten = 0; 
  for ( size_t i = 0; i < size; i++ )
  {
    if ( bytesWritten >= width )
    {
      fputc( '\n', stream );
      bytesWritten = 0;
    }
    bytesWritten += fprintf( stream, "%5.2f ", arr[i] );
  }
  fputc( '\n', stream );
}

#define SIZE 100000

int main( void )
{
  srand( time( NULL ));
  double *x = malloc( sizeof *x * SIZE );

  double *y = malloc( sizeof *y * SIZE );

  void (*f[])(unsigned int, double *) = {hpsort1, hpsort2};
  char *names[] = {"hpsort1", "hpsort2"};

  for ( size_t i = 0; i < 20; i++ )
  {
    int r = rand() % 2;
    init ( x, SIZE);

    memcpy( y, x, sizeof *x * SIZE );
    clock_t start, end;
    start = clock();
    f[r]( SIZE, y-r );
    end = clock();

    printf( "%s time: %f\t", names[r], (double) (end - start) / CLOCKS_PER_SEC );

    r = !r;
    memcpy( y, x, sizeof *x * SIZE);
    start = clock();
    f[r]( SIZE, y-r );
    end = clock();

    printf( "%s time: %f\n", names[r], (double) (end - start) / CLOCKS_PER_SEC );
  }

  return 0;
}

