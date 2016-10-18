#include <iostream>
#include <GpuInfo.h>

using namespace std;
using namespace gpuinfo::iostream;

int main(int argc, char *argv[])
{
  const auto& devices = gpuinfo::devices();
  for (const auto& device : devices) {
    std::cout << "Name: " << device->name() << std::endl;
    std::cout << "Memory: " << device->memory() << std::endl;
    std::cout << "Thermal sensors:" << std::endl;
    for (const auto& sensor : device->thermal_sensors()) {
      std::cout << '\t' << sensor << std::endl;
    }
  }
  return 0;
}
