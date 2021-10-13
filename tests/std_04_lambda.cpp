/*
 * std::thread and lambda capturing
 */
#include <array>
#include <thread>
#include <string>

#include <base/verify.h>

using namespace std;

int main(int argc, char *argv[])
{
  using namespace std;

  bool local = false;
  auto fn = [&]() {
    local = true;
  };

  std::thread th {fn};
  th.join();

  VERIFY(local);
  return 0;
}

/* EOF */
