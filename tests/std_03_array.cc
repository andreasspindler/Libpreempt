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

class Task
{
public:
  void operator()() const { }

  void operator()(std::string arg) const { VERIFY(arg == "example"); }

  void operator()(int n) const { VERIFY(n == 42); }
} task;

int main(int argc, char *argv[])
{
  try {
    /*
     * std::thread[]
     */
    std::thread tha1[] {                          // thread array
      std::thread {task},                         // Task::operator()
      std::thread {task, std::string("example")}, // Task::operator(string)
      std::thread {task, "example"},              // Task::operator(string)
      std::thread {task, 42},                     // Task::operator(int)
#if 1
      /* "C++’s most vexing parse." If you pass a temporary rather than a named
         variable, then the syntax can be the same as that of a function
         declaration, in which case the compiler interprets it as such, rather
         than an object definition. */
      std::thread {Task(), 42}
#endif
    };

    /*
     * std::array<std::thread, SIZE>
     */
    std::array<std::thread, 10> tha2;
    for (auto& th : tha2) {
      //i = thread(task, "work");
      VERIFY(!th.joinable());
    }

    for (auto& th : tha2) {
      th = std::thread(task, "example");
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

  return global_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
