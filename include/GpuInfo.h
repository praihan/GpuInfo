#ifndef GPUINFO_H
#define GPUINFO_H

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <utility>

namespace gpuinfo {
  using u64 = std::uint64_t;
  using u32 = std::uint32_t;
  using i64 = std::int64_t;
  using i32 = std::int32_t;

  inline namespace util {
    template <unsigned Bit>
    constexpr u64 bit() {
      static_assert(Bit < 64, "I don't know how to go past 64 bits");
      return u64(1) << Bit;
    }
  }

  struct memory_info {
    u32 dedicated;
    u32 available_dedicated;
    u32 system;
    u32 shared_system;
  };
  std::ostream& operator<<(std::ostream&, const memory_info&);

  enum class thermal_sensor : u64 {
    unknown = 0,
    gpu = bit<0>(),
    memory = bit<1>(),
    power_supply = bit<2>(),
    ambient = bit<3>(),
  };
  struct thermal_sensor_info {
    i32 current_temp;
    thermal_sensor sensor_type;
  };
  std::ostream& operator<<(std::ostream&, const thermal_sensor_info&);

  struct device {
    virtual std::string name() const = 0;
    virtual memory_info memory() const = 0;
    virtual std::vector<thermal_sensor_info> thermal_sensors() const = 0;
  };

  const std::vector<std::shared_ptr<const device>>& devices();
}

#endif // GPUINFO_H
