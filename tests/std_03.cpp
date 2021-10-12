/*
 * std::thread background task executing nonstatic member function
 */
#include <thread>
#include <string>
#include <cassert>
#include <memory>

using namespace std;

class Task {
public:
  void
  execute(std::string arg) {
    assert(arg == "example");
  }
};

int main(int argc, char *argv[])
{
  {                             // Task::execute via object on stack
    Task task;
    thread th {&Task::execute, &task, "example"};
    th.join();
  }
  {                             // Task::execute via raw pointer
    Task *task = new Task;
    thread th {&Task::execute, task, "example"};
    th.join();
    delete task;
  }
  {                             // Task::execute via smart pointer
    auto task = make_shared<Task>();
    thread th {&Task::execute, task, "example"};
    th.join();
  }
}

