/*
 * std_01_thread.cpp
 *
 * std::thread background task executing global function and static class member
 * function.
 */
#include <vector>
#include <thread>
#include <string>

#include <base/debug.h>

#define X "Om Gam Ganapataye Namaha"

void* Task1(void* arg) {
  return nullptr;
}

void* Task2(char const* arg) {
  /* The VERIFY macro works like assert() but also if NDEBUG is
     defined. */
  VERIFY(std::string(arg) == X);
  return nullptr;
}

void Task3(char const* arg, int n) {
  VERIFY(n == 42);
  Task2(arg);
}

class Task4 {
public:
  static void execute(std::string arg) {
    VERIFY(arg == "example");
  }
};

int main(int argc, char *argv[])
{
  /* Create array of threads running functions Task1(), Task2() and
     Task3(). The execution order is undefined! */
  std::thread tha[] {
    std::thread {Task1, nullptr},
    std::thread {Task1, (void*) X},
    std::thread {Task2, X},
    std::thread {Task3, X, 42}
  };

  /* Static class member function Task4::execute() */
  std::thread th {&Task4::execute, "example"};

  /* The destructor of std::thread invokes std::terminate when the
     thread wasn't joined or detached. */
  th.join();
  for (std::thread& th : tha) {
    th.join();
  }
}
