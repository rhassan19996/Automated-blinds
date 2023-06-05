/*
Md Hassan
Write three functions in C or C++: one that declares a large array statically, one that declares the same large
array on the stack, and one that creates the same large array from the heap.  Call each of the
subprograms a large number of times (at least 100,000) and output the time required by each. Explain the results.
*/

#include <iostream>
#include <time.h>

using namespace std;

void static_array();
void stack_array();
void heap_array();

int main(int argc, char const *argv[])
{
  int count = 100000;
  cout << "This program measures the time to execute the different array sizes" << endl;
  cout << "---------------------------------------------------------------------"
       << "\n"
       << endl;
  cout << "First we'll measure the time to execute the static array! "
       << "\n"
       << endl;

  // displays the time run static array
  clock_t start, end;
  start = clock();
  for (int i = 0; i < count; i++)
  {
    static_array();
  }
  end = clock();
  double duration_sec = double(end - start) / CLOCKS_PER_SEC;
  printf("static array took %f seconds to execute \n", duration_sec);
  cout << "---------------------------------------------------------------------"
       << "\n"
       << endl;
  // Displays the time to execute the stack array

  start = clock();
  for (int i = 0; i < count; i++)
  {
    stack_array();
  }
  end = clock();
  double duration_sec2 = double(end - start) / CLOCKS_PER_SEC;
  printf("Stack array took %f seconds to execute \n", duration_sec2);
  cout << "---------------------------------------------------------------------"
       << "\n"
       << endl;

  start = clock();
  for (int i = 0; i < count; i++)
  {
    heap_array();
  }
  end = clock();
  double duration_sec3 = double(end - start) / CLOCKS_PER_SEC;
  printf("heap array took %f seconds to execute \n", duration_sec3);
  cout << "---------------------------------------------------------------------"
       << "\n"
       << endl;

  return 0;
}
// end of main function

void static_array(void)
{
  static int array1[200000];
}

void stack_array()
{
  int array2[200000];
}

void heap_array()
{
  int *array3 = (int *)malloc(200000 * sizeof(int));
}