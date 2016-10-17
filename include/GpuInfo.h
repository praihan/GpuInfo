#ifndef GPUINFO_H
#define GPUINFO_H

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace gpuinfo {
  using u64 = std::uint64_t;
  using u32 = std::uint32_t;
  using i64 = std::int64_t;
  using i32 = std::int32_t;

  struct memory_info {
    u32 dedicated;
    u32 available_dedicated;
    u32 system;
    u32 shared_system;
  };

  struct thermal_sensor_info {
    u32 index;
    i32 max;
    i32 min;
    i32 current;
  };

  struct device {
    virtual std::string name() const = 0;
    virtual memory_info memory() const = 0;
    virtual std::vector<thermal_sensor_info> thermal_sensors() const = 0;
  };

  const std::vector<std::shared_ptr<const device>>& devices();
}

#endif // GPUINFO_H
