#ifndef GPUINFO_H
#define GPUINFO_H

/// \file

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <utility>

namespace gpuinfo {
  /// The unsigned 64 bit integer type used for convenience in this library.
  using u64 = std::uint64_t;
  /// The unsigned 32 bit integer type used for convenience in this library.
  using u32 = std::uint32_t;
  /// The signed 64 bit integer type used for convenience in this library.
  using i64 = std::int64_t;
  /// The signed 32 bit integer type used for convenience in this library.
  using i32 = std::int32_t;

  inline namespace util {
    ///
    /// Returns an unsigned integer with only the `Bit`-th bit set.
    ///
    /// \return `1 << Bit`.
    /// \pre `Bit` must be less than `64`.
    ///
    template <unsigned Bit>
    constexpr u64 bit() {
      static_assert(Bit < 64, "I don't know how to go past 64 bits");
      return u64(1) << Bit;
    }
  }

  ///
  /// A plain-old-data structure containing information about a device's memory.
  ///
  struct memory_info {
    /// Size (in kB) of the physical framebuffer.
    u32 dedicated;
    /// Size (in kB) of the available physical framebuffer for allocating video memory surfaces.
    u32 available_dedicated;
    /// Size (in kB) of system memory that is allocated at driver load time.
    u32 system;
    /// Size (in kB) of total shared system memory that is available for all surface allocations.
    u32 shared_system;
  };
  static_assert(std::is_pod<memory_info>::value, "memory_info should be POD, what happened?");

  ///
  /// An enumeration of the types of targets a thermal sensor could be pointed at.
  ///
  enum class thermal_sensor : u64 {
    /// A thermal sensor without a known target.
    unknown = 0,
    /// A thermal sensor targetting the GPU.
    gpu = bit<0>(),
    /// A thermal sensor targetting the device's memory.
    memory = bit<1>(),
    /// A thermal sensor targetting the device's power supply.
    power_supply = bit<2>(),
    /// A thermal sensor targetting the device's ambience (the board).
    ambient = bit<3>(),
  };
  struct thermal_sensor_info {
    /// Current sensor temperature reading (in degrees Celsius).
    i32 current_temp;
    /// The target of this thermal sensor.
    thermal_sensor sensor_type;
  };
  static_assert(std::is_pod<thermal_sensor_info>::value, "thermal_sensor_info should be POD, what happened?");

  ///
  /// A device represents a physical device in the hardware.
  ///
  /// All access to device information is through instances of this class.
  /// To access the physical devices attached to this computer, see `devices()`.
  /// \see devices()
  ///
  struct device {
    /// Gets the full name of the physical device.
    virtual std::string name() const = 0;
    /// Reads the current information about the memory on the device.
    virtual memory_info memory() const = 0;
    /// Reads the current information about the thermal sensors on the device.
    virtual std::vector<thermal_sensor_info> thermal_sensors() const = 0;
  };

  ///
  /// Gets an array of the physical devices attached to this computer. The list of
  /// physical devices is queried only once: when this function is called for the first time. 
  ///
  const std::vector<std::shared_ptr<const device>>& devices();

  namespace iostream {
    ///
    /// Prints a memory_info struct in a human-readable form to an output stream.
    ///
    std::ostream& operator<<(std::ostream&, const memory_info&);

    ///
    /// Prints a thermal_sensor_info struct in a human-readable form to an output stream.
    ///
    std::ostream& operator<<(std::ostream&, const thermal_sensor_info&);
  }
}

#endif // GPUINFO_H
