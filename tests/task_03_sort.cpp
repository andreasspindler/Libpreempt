/* -*- coding: raw-text-unix; -*-
 *
 * Find out execution time of realtime threads.
 *
 * In each thred generate and sort a random integer vector within a given number
 * of milliseconds, or fail.
 */
#include "common.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

template <int Ms>
struct CriticalSort : CriticalTask<Ms> {
  using value_type = int;
  vector<value_type> data;

  CriticalSort(int size)
    : data(size) { }

  void run() override {
    srand(time(nullptr));
    // for (auto& i : data) { i = rand() % data.size() + 1; }
    generate(data.begin(), data.end(),
             [this]()->value_type { return rand() % data.size() + 1; });
    sort(data.begin(), data.end());
  }
};

int
main(int argc, char *argv[])
{
  CriticalSort<3> t { 1000};
  CriticalSort<5> u {10000};

  t.start(3);
  u.start(2);

  t.join(); PRINT(t);
  u.join(); PRINT(u);

  return EXIT_SUCCESS;
}
