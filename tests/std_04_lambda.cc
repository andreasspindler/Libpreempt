/*
 * std::thread and lambda capturing
 */
#include <array>
#include <thread>
#include <string>

#include <base/verify.h>

int main(int argc, char *argv[])
{
  bool local = false;
  auto fn = [&]() {
    local = true;
  };

  std::thread th {fn};
  th.join();

  VERIFY(local);

  return global_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
