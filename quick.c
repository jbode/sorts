#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

static size_t gcompare=0;
static size_t gswap=0;
static size_t gsortlevel = 0;
static size_t grecdepth = 0;

int validate( const int * const x, size_t size )
{
  for ( size_t i = 0; i < size-1; i++ )
    if ( x[i] > x[i+1] )
      return 0;
  return 1;
}

void display( const char * const prompt, int * const x, size_t size )
{
  int tot = 0;
  int dig = (int)(ceil(log10((double) size)));
  char fmt[20];
  sprintf( fmt, "%%%dd ", dig );
  printf( "%s:\n", prompt );
  for ( size_t i = 0; i < size; i++ )
  {
    tot += printf( fmt, x[i] );
    if ( tot >= 120 )
    {
      putchar( '\n' );
      tot = 0;
    }
  }
  putchar( '\n' ); 
  fflush( stdout );
} 

inline int cmp( int x, int y )
{
  gcompare++;
  return x - y;
}

inline void swap( int * const restrict x, int * const restrict y )
{
  gswap++;

  int t = *x;
  *x = *y;
  *y = t;
}

void myins( int * const x, size_t lo, size_t hi, size_t (*pfunc)( int * const, size_t, size_t ) )
{
  for ( size_t i = lo + 1; i <= hi; i++ )
  {
    int j = i;
    while ( j > lo && cmp( x[j-1], x[j] ) > 0 )
    {
      swap( &x[j], &x[j-1] );
      j--;
    }
  } 
}

void mysel( int * const x, size_t lo, size_t hi, size_t (*pfunc)( int * const, size_t, size_t ) )
{
  for ( size_t j = hi; j > lo; j-- )
  {
    int max = x[j];
    size_t idx = j;

    for ( size_t i = lo; i < j; i++ )
    {
      if ( cmp( x[i], max ) > 0 )
      {
        max = x[i];
        idx = i;
      }
    }
    if ( idx != j )
      swap( &x[idx], &x[j] );
  }   
}

void mybub( int * const x, size_t lo, size_t hi, size_t (*pfunc)( int * const, size_t, size_t ) )
{
  for ( size_t i = lo; i <= hi - 1; i++ )
    for ( size_t j = i; j <= hi; j++ )
      if ( cmp( x[i], x[j]) > 0 )
        swap( &x[i], &x[j] );
}

size_t bode( int * const x, size_t lo, size_t hi )
{
  size_t i = lo, j = hi;
  while ( i <= j )
  {
    while ( i <= j && cmp( x[i], x[j] ) <= 0 )
      i++;
 
    if ( i <= j )
      swap( &x[i], &x[j] );

    while ( j > i && cmp(x[j], x[i]) > 0 )
      j--;

    if ( i <= j )
      swap( &x[i], &x[j] );
  }
  return i;
}

int median( int a, int b, int c )
{
  int result = b;

  if ( a >= b && b >= c )
    result = b;

  if ( b >= a && a >= c )
    result = a;

  if ( a >= c && c >= b )
    result = c;

  return result;
}

size_t hoare( int * const x, size_t lo, size_t hi )
{
  long int i = lo - 1, j = hi + 1;
  int pivot = x[lo]; // median( x[0], x[size/2], x[size-1] );

  for( ;; )
  {
    do { i++; } while ( cmp( x[i], pivot ) < 0 );
    do { j--; } while ( cmp( x[j], pivot ) > 0 );

    if ( i >= j )
      break;

    swap( &x[i], &x[j] );
  }
  return j;
}
  
void myqsort( int * const x, size_t lo, size_t hi, size_t (*pfunc)( int * const, size_t, size_t ) )
{
  if ( ((hi - lo) + 1) < 2 )
    return;

  gsortlevel++;
  if ( gsortlevel > grecdepth )
    grecdepth++;

  size_t part = pfunc( x, lo, hi );

  if ( part < hi )
  {
    myqsort( x, lo, part, pfunc );
    myqsort( x, part + 1, hi, pfunc );
  }
  gsortlevel--;
}

void init( int * const x, size_t SIZE )
{
  for ( size_t i = 0; i < SIZE; i++ )
    x[i] = rand() % SIZE;
}

int main( int argc, char **argv )
{
  unsigned long size = 100;
  size_t lo = 0, hi = size - 1;
  void (*sort)( int * const, size_t, size_t, size_t (*)(int * const, size_t, size_t) ) = NULL;
  size_t (*part)( int * const, size_t, size_t ) = NULL;

  int opt;
  int seed = 0; // do not use random seed;
  int resultsOnly = 0; // do not display array contents
  int sortPicked = 0;

  while ( (opt = getopt( argc, argv, "n:srBSIQp:" ) ) != -1 )
  {
    switch( opt )
    {
      case 'n':
        size = strtoul( optarg, NULL, 10 );
        break;

      case 'r':
        resultsOnly = 1;
        break;

      case 's':
        seed = 1;
        break;

      case 'B':
        sort = mybub;
        sortPicked = 1;
        break;

      case 'S':
        sort = mysel;
        sortPicked = 1;
        break;

      case 'I':
        sort = myins;
        sortPicked = 1;
        break;

      case 'Q':
        sort = myqsort;
        part = hoare;
        sortPicked = 1;
        break;

      case 'p':
        switch( optarg[0] )
        {
          default:
          case 'h':
            part = hoare;
            break;

          case 'b':
            part = bode;
            break;
        }
        break;
    }
  }
    
  if ( !sortPicked )
  {
    fprintf( stderr, "USAGE: %s {-B|-S|-Q [-p b|h] |-I} [-n size] [-s] [-r]\n", argv[0] );
    exit( EXIT_FAILURE );
  }

  if ( seed )
    srand( time( NULL ));

  int *source = malloc( sizeof *source * size );
  int *work = malloc( sizeof *work * size );
  init( source, size );

  lo = 0;
  hi = size - 1;

  gcompare = gswap = 0;
  memcpy( work, source, size * sizeof *work );
  if ( !resultsOnly )
    display( "before sort", work, size );

  clock_t clockStart = clock();
  sort( work, lo, hi, part );
  clock_t clockEnd = clock();

  if ( !resultsOnly )
    display( "after sort", work, size );
  if ( validate( work, size ) )
    printf( "elements: %lu\tcomparisons: %zu\tswaps: %zu\truntime: %lu clocks (%f secs)\tmax recur: %zu\n\n", size, gcompare, gswap,
      (unsigned long) (clockEnd - clockStart), (double)(clockEnd - clockStart)/CLOCKS_PER_SEC, grecdepth  );
  else
    printf( "sort algorithm failed\n" );

  free( work );
  free( source );

  return 0;
}

  
