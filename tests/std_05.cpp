/*
 * std::thread background task as array of callables
 */
#include <array>
#include <thread>
#include <string>
#include <cassert>

using namespace std;

class Task
{
public:
  void operator()(std::string arg) const
    {
      assert(arg == "work");
    }
};

int main(int argc, char *argv[])
{
  using namespace std;
  Task task;

  array<thread, 10> a;
  for (auto& i : a) { i = thread(task, "work"); }
  for (auto& i : a) { i.join(); }
  return 0;
}
