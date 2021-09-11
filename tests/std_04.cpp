/*
 * std::thread background task executing callable (std::is_invocable)
 *
 * std::thread works with any callable type, object or function. The overload of
 * 'operator()' of its class makes an object callable.
 *
 */
#include <thread>
#include <string>
#include <cassert>

using namespace std;

class Task
{
public:
  void operator()() const
    {
    }

  void operator()(std::string arg) const
    {
      assert(arg == "example");
    }

  void operator()(int n) const
    {
      assert(n == 42);
    }
};

int main(int argc, char *argv[])
{
  using namespace std;
  Task task;
  thread tha[] {
    thread {task},                         // Task::operator()
    thread {task, std::string("example")}, // Task::operator(string)
    thread {task, "example"},              // Task::operator(string)
    thread {task, 42},                     // Task::operator(int)
#if 1
  /* “C++’s most vexing parse.” If you pass a temporary rather than a
     named variable, then the syntax can be the same as that of a
     function declaration, in which case the compiler interprets it as
     such, rather than an object definition. */
    thread {Task(), 42}
#endif
  };
  for (thread& th : tha) {
    th.join();
  }
  return 0;
}
