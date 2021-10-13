/*
 * std::thread background task executing global function
 */
#include <thread>
#include <string>

#include <base/verify.h>

using namespace std;

#define X "Om Gam Ganapataye Namaha"

void* Task1(void* arg) {
  return nullptr;
}

void* Task2(char const* arg) {
  VERIFY(std::string(arg) == X);
  return nullptr;
}

void Task3(char const* arg, int n) {
  VERIFY(n == 42);
  Task2(arg);
}

int main(int argc, char *argv[])
{
  /* Create array of threads. The execution order is undefined. */
  thread tha[] {
    thread {Task1, nullptr},
    thread {Task1, (void*) X},
    thread {Task2, X},
    thread {Task3, X, 42}
  };

  /* The destructor of std::thread invokes std::terminate when the thread wasn't
     joined or detached. */
  for (thread& th : tha) {
    th.join();
  }
  return 0;
}
