/* -*- coding: raw-text-unix; -*-
 *
 * Find out execution time of realtime threads.
 *
 * In each thread generate and sort a random integer vector within a
 * given number of milliseconds, or fail.
 */
#include "common.h"
#include <array>
#include <base/algorithm.h>

using namespace std;

template <int Ms, int Size>
struct DefaultSort : CriticalTask<Ms> {
  array<int, Size> data;
  void run() override {
    srand(time(nullptr));
    // for (auto& i : data) { i = rand(); }
    generate(data.begin(), data.end(), [this]()->int { return rand(); });
    sort(data.begin(), data.end());
  }
};

template <int Ms, int Size>
struct BubbleSort : CriticalTask<Ms> {
  array<int, Size> data;
  void run() override {
    srand(time(nullptr));
    generate(data.begin(), data.end(), [this]()->int { return rand(); });
    base::bubble_sort(data);
  }
};

int
main(int argc, char *argv[])
{

  DefaultSort<3, 1000> t;       // sort within 3ms or fail
  BubbleSort< 3,  100> u;

  t.start();
  u.start();

  t.join(); PRINT(t);           // see task_03_sort.stdout
  u.join(); PRINT(u);

  return EXIT_SUCCESS;
}
