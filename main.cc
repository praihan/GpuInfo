#include <iostream>
#include <GpuInfo.h>

using namespace std;

int main(int argc, char *argv[])
{
  const auto& devices = gpuinfo::devices();
  for (const auto& device : devices) {
    std::cout << device->name() << std::endl;
  }
  return 0;
}
