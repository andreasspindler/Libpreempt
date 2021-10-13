/*
 * std_01_thread.cpp
 *
 * std::thread background task executing global function and static class member
 * function.
 */
#include <vector>
#include <thread>
#include <string>

#include <base/verify.h>

#define X "Om Gam Ganapataye Namaha"

using namespace std;

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

class Task4 {
public:
  static
  void
  execute(std::string arg) {
    VERIFY(arg == "example");
  }
};

int main(int argc, char *argv[])
{
  /*
   * Create array of threads running functions Task1(), Task2() and Task3(). The
   * execution order is undefined.
   */
  thread tha[] {
    thread {Task1, nullptr},
    thread {Task1, (void*) X},
    thread {Task2, X},
    thread {Task3, X, 42}
  };

  /*
   * Task4::execute() - static class member function
   */
  thread th {&Task4::execute, "example"};

  /* The destructor of std::thread invokes std::terminate when the thread wasn't
     joined or detached. */
  th.join();
  for (thread& th : tha) {
    th.join();
  }
}
