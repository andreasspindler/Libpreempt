/*
 * std::thread background task executing static member function
 */
#include <vector>
#include <thread>
#include <string>
#include <cassert>

using namespace std;

class Task {
public:
  static
  void
  execute(std::string arg) {
    assert(arg == "example");
  }
};

int main(int argc, char *argv[])
{
  thread th {&Task::execute, "example"};
  th.join();
}
