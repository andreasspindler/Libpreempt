/*
 * std::thread array executing callables (std::is_invocable)
 *
 * std::thread works with any callable type. The overload of 'operator()' of its
 * class makes an object callable.
 */
#include <thread>
#include <string>
#include <array>

#include <exception>
#include <iostream>

#include <base/verify.h>

using namespace std;

class Task
{
public:
  void operator()() const
    {
    }

  void operator()(std::string arg) const
    {
      VERIFY(arg == "example");
    }

  void operator()(int n) const
    {
      VERIFY(n == 42);
    }
} task;

int main(int argc, char *argv[])
{
  using namespace std;

  try {
    /*
     * std::thread[]
     */
    thread tha1[] {               // thread array
      thread {task},              // Task::operator()
      thread {task, std::string("example")}, // Task::operator(string)
      thread {task, "example"},              // Task::operator(string)
      thread {task, 42},                     // Task::operator(int)
#if 1
      /* "C++’s most vexing parse." If you pass a temporary rather than a named
         variable, then the syntax can be the same as that of a function
         declaration, in which case the compiler interprets it as such, rather
         than an object definition. */
      thread {Task(), 42}
#endif
    };

    /*
     * std::array<std::thread, SIZE>
     */
    array<thread, 10> tha2;
    for (auto& th : tha2) {
      //i = thread(task, "work");
      VERIFY(!th.joinable());
    }

    for (auto& th : tha2) {
      th = thread(task, "example");
      VERIFY(th.joinable());
    }

    /*
     * join both arrays
     */
    for (auto& th : tha1) { th.join(); }
    for (auto& th : tha2) { th.join(); }
  } catch (std::runtime_error const& ex) {
    /*
     * trying to join() a default-constructed thread may throw a
     * std::system_error
     */
    std::cerr << "catched: " << ex.what() << std::endl;
    std::terminate();
  }
  return 0;
}
