/*
 * std_02_nonstatic.cpp
 *
 * std::thread background task executing nonstatic member function
 */
#include <thread>
#include <string>
#include <memory>
#include <chrono>

#include <base/debug.h>

#define X "Om Sharavanabhavaya Namaha"

int main(int argc, char *argv[])
{
  /*
   * Class method as thread.
   */
  {
    class Task {
    public:
      void run(std::string arg) {
        VERIFY(arg == X);
      }
    };
    {                             // Task::run via object on stack
      Task task;
      std::thread th {&Task::run, &task, X};
      th.join();
    }
    {                             // Task::run via raw pointer
      Task *task = new Task;
      std::thread th {&Task::run, task, X};
      th.join();
      delete task;
    }
    {                             // Task::run via smart pointer
      auto task = std::make_shared<Task>();
      std::thread th {&Task::run, task, X};
      th.join();
    }
  }

  /*
   * Class that starts threads per member function. We simply use an explicit
   * start() method. Without such a method each thread would have to wait for
   * the ctor to finish (using a condition variable and a mutex).
   */
  {                             // Task with multiple threads
    class Task {
    public:
      ~Task()
        {
          if (foo_thread_.joinable()) foo_thread_.join();
          if (bar_thread_.joinable()) bar_thread_.join();
        }

      void start() {
        foo_thread_ = std::thread {[this]() { foo(); }};
        bar_thread_ = std::thread {[this]() { bar(); }};
      }

      void foo() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }

      void bar() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
      }

    private:
      std::thread foo_thread_;
      std::thread bar_thread_;
    };

    Task().start();
  }

  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
