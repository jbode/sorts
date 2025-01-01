#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

/**
 * Some useful statistics to capture -
 * gcompare   - total number of comparisons
 * gswap      - total number of swaps
 * gsortlevel - current recursion level
 * grecdepth  - maximum recursion level
 */
static size_t gcompare    = 0;
static size_t gswap       = 0;
static size_t gsortlevel  = 0;
static size_t grecdepth   = 0;

/**
 * Check that the array is sorted
 */
int validate( const double * const x, size_t size )
{
  for ( size_t i = 0; i < size-1; i++ )
    if ( isgreater(x[i], x[i+1]) )
      return 0;
  return 1;
}

/**
 * Display the contents of the array, with nicely formatted output.
 */
void display( const char * const prompt, double * const x, size_t size, int width, int prec )
{
  int tot = 0;
  printf( "%s:\n", prompt );
  for ( size_t i = 0; i < size; i++ )
  {
    tot += printf( "%*.*f ", width, prec, x[i] );
    if ( tot >= 120 )
    {
      putchar( '\n' );
      tot = 0;
    }
  }
  putchar( '\n' ); 
  fflush( stdout );
} 

/**
 * Comparison function - counts the number of comparisons, can also
 * be overidden to change the sense of the test
 */
static inline int cmp( double x, double y )
{
  gcompare++;
  return isgreater(x,y) ? 1 : (isless(x,y) ? -1 : 0);
}

/**
 * Swap function - counts the number of swaps
 */
static inline void swap( double * const restrict x, double * const restrict y )
{
  gswap++;

  double t = *x;
  *x = *y;
  *y = t;
}

/**
 * Insertion sort.
 */
void myins( double * const x, size_t lo, size_t hi, size_t (*pfunc)( double * const, size_t, size_t ) )
{
  for ( size_t i = lo + 1; i <= hi; i++ )
  {
    size_t j = i;
    while ( j > lo && cmp( x[j-1], x[j] ) > 0 )
    {
      swap( &x[j], &x[j-1] );
      j--;
    }
  } 
}

/**
 * Selection sort
 */
void mysel( double * const x, size_t lo, size_t hi, size_t (*pfunc)( double * const, size_t, size_t ) )
{
  for ( size_t j = hi; j > lo; j-- )
  {
    double max = x[j];
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

/**
 * Bubble sort
 */
void mybub( double * const x, size_t lo, size_t hi, size_t (*pfunc)( double * const, size_t, size_t ) )
{
  for ( size_t i = lo; i <= hi - 1; i++ )
    for ( size_t j = i; j <= hi; j++ )
      if ( cmp( x[i], x[j]) > 0 )
        swap( &x[i], &x[j] );
}

/**
 * Computes the median of three values - used
 * by quicksort partion function
 */
double median( double a, double b, double c )
{
  double result = b;

  if ( a >= b && b >= c )
    result = b;

  if ( b >= a && a >= c )
    result = a;

  if ( a >= c && c >= b )
    result = c;

  return result;
}

/**
 * Standard Hoare partitioning function for quicksort
 */
size_t hoare( double * const x, size_t lo, size_t hi )
{
  long int i = lo - 1, j = hi + 1;
  double pivot = x[lo]; // median( x[0], x[size/2], x[size-1] );

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

/**
 * Quicksort
 */
void myqsort( double * const x, size_t lo, size_t hi, size_t (*pfunc)( double * const, size_t, size_t ) )
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

/**
 * Write randomly ordered values to an array
 */
void init( double * const x, size_t size )
{
  for ( size_t i = 0; i < size; i++ )
    x[i] = rand()/100.0;
}

/**
 * Main
 */
int main( int argc, char **argv )
{
  unsigned long size = 100;
  size_t lo = 0, hi = size - 1;
  void (*sort)( double * const, size_t, size_t, size_t (*)(double * const, size_t, size_t) ) = NULL;
  size_t (*part)( double * const, size_t, size_t ) = NULL;

  int opt;
  int seed = 0; // do not use random seed;
  unsigned int seedval = 0; // explicit seed value
  int resultsOnly = 0; // do not display array contents
  int sortPicked = 0;
  int displayWidth = 15;
  int displayPrec = 2;

  while ( (opt = getopt( argc, argv, "n:s:rBSIQp:W:P:" ) ) != -1 )
  {
    switch( opt )
    {
      case 'n':
        if ( !optarg )
        {
          fprintf( stderr, "n requires a value\n" );
          exit( 0 );
        }
        size = strtoul( optarg, NULL, 10 );
        break;

      case 'r':
        resultsOnly = 1;
        break;

      case 's':
        seed = 1;
        if ( optarg )
        {
           seedval = (unsigned int) strtoul( optarg, NULL, 0 );
        }
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
        }
        break;
        
      case 'W':
        if ( !optarg )
        {
          fprintf( stderr, "W requires a value\n" );
          exit( 0 );
        }
        displayWidth = strtol( optarg, NULL, 10 );
        break;

      case 'P':
        if ( !optarg )
        {
          fprintf( stderr, "O requires a value\n" );
          exit( 0 );
        }
        displayPrec = strtol( optarg, NULL, 10 );
        break;

      case ':':
      case '?':
        if ( optopt == 's' )
        {
          seed = 1;
        }
        else
        {
          fprintf( stderr, "Option %c requires an argument\n", optopt );
          exit(0);
        }
    }
  }
    
  if ( !sortPicked )
  {
    fprintf( stderr, "USAGE: %s {-B|-S|-Q [-p b|h] |-I} [-n size] [-s [sval]] [-r]\n", argv[0] );
    exit( EXIT_FAILURE );
  }

  if ( seed )
  {
    if ( seedval == 0 )
      srand( time( NULL ) );
    else
      srand( seedval );
  }
  
  double *source = malloc( sizeof *source * size );
  double *work = malloc( sizeof *work * size );
  init( source, size );

  lo = 0;
  hi = size - 1;

  gcompare = gswap = 0;
  memcpy( work, source, size * sizeof *work );
  if ( !resultsOnly )
    display( "before sort", work, size, displayWidth, displayPrec );

  clock_t clockStart = clock();
  sort( work, lo, hi, part );
  clock_t clockEnd = clock();

  if ( !resultsOnly )
    display( "after sort", work, size, displayWidth, displayPrec );
  if ( validate( work, size ) )
    printf( "elements: %lu\tcomparisons: %zu\tswaps: %zu\truntime: %lu clocks (%f secs)\tmax recur: %zu\n\n", size, gcompare, gswap,
      (unsigned long) (clockEnd - clockStart), (double)(clockEnd - clockStart)/CLOCKS_PER_SEC, grecdepth  );
  else
    printf( "sort algorithm failed\n" );

  free( work );
  free( source );

  return 0;
}

  
