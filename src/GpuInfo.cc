/// \file

#include <GpuInfo.h>

#include <Windows.h>
#include <nvapi.h>

#include <utility>
#include <functional>
#include <memory>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <cassert>

namespace gpuinfo {

  /// This anonymous namespace is for general utility functions
  namespace {
    ///
    /// Gets a canonical string to represent a `thermal_sensor`.
    /// \internal
    ///
    const char* thermal_sensor_type_to_string(thermal_sensor sensor_target) {
      switch (sensor_target) {
      case thermal_sensor::gpu:
        return "gpu";
      case thermal_sensor::memory:
        return "memory";
      case thermal_sensor::power_supply:
        return "power_supply";
      case thermal_sensor::ambient:
        return "ambient";
      case thermal_sensor::unknown:
        return "unknown";
      default:
        return "<multiple>";
      }
    }
  }

  /// This anonymous namespace is for NVIDIA-specific things
  namespace {
    ///
    /// Gets the `thermal_sensor` corresponding to an `NV_THERMAL_TARGET`.
    /// \internal
    ///
    thermal_sensor _nv_thermal_target_convert(NV_THERMAL_TARGET target) {
      switch (target) {
      case NVAPI_THERMAL_TARGET_GPU:
        return thermal_sensor::gpu;
      case NVAPI_THERMAL_TARGET_MEMORY:
        return thermal_sensor::memory;
      case NVAPI_THERMAL_TARGET_POWER_SUPPLY:
        return thermal_sensor::power_supply;
      case NVAPI_THERMAL_TARGET_BOARD:
        return thermal_sensor::ambient;
      default:
        return thermal_sensor::unknown;
      }
    }

    ///
    /// The `nvidia_device` structure is an implementation of `device` for NVIDIA devices
    /// using NVAPI.
    ///
    struct nvidia_device : device {
      ///
      /// Initializes an `nvidia_device` from an NVAPI handle to a physical device.
      ///
      nvidia_device(NvPhysicalGpuHandle gpu_handle) : _gpu_handle(gpu_handle) {
        // TODO: handle error
        assert(this->_gpu_handle != nullptr);
      }

      ///
      /// \copydoc device::name()
      ///
      virtual std::string name() const override {
        NvAPI_ShortString nv_name;
        NvAPI_Status status = NVAPI_OK;

        status = NvAPI_GPU_GetFullName(this->_gpu_handle, nv_name);
        if (status != NVAPI_OK) {
          // TODO: handle error
          assert(false);
        }
        return std::string{ nv_name };
      }

      ///
      /// \copydoc device::memory()
      ///
      virtual memory_info memory() const override {
        NvAPI_Status status = NVAPI_OK;
        NV_DISPLAY_DRIVER_MEMORY_INFO nv_memory_info;

        nv_memory_info.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;

        status = NvAPI_GPU_GetMemoryInfo(this->_gpu_handle, std::addressof(nv_memory_info));
        if (status != NVAPI_OK) {
          // TODO: handle error
          assert(false);
        }

        memory_info mem_info;
        mem_info.system              = nv_memory_info.systemVideoMemory;
        mem_info.shared_system       = nv_memory_info.sharedSystemMemory;
        mem_info.dedicated           = nv_memory_info.dedicatedVideoMemory;
        mem_info.available_dedicated = nv_memory_info.availableDedicatedVideoMemory;

        return mem_info;
      }

      ///
      /// \copydoc device::thermal_sensors()
      ///
      virtual std::vector<thermal_sensor_info> thermal_sensors() const override {
        NV_GPU_THERMAL_SETTINGS nv_thermal_settings;
        NvAPI_Status status = NVAPI_OK;

        nv_thermal_settings.version = NV_GPU_THERMAL_SETTINGS_VER;

        status = NvAPI_GPU_GetThermalSettings(this->_gpu_handle, NVAPI_THERMAL_TARGET_ALL, std::addressof(nv_thermal_settings));
        if (status != NVAPI_OK) {
          // TODO: handle error
          assert(false);
        }

        const std::size_t sensors_count = nv_thermal_settings.count;

        std::vector<thermal_sensor_info> sensor_infos;
        sensor_infos.reserve(sensors_count);

        // This type is anonymous, so we give it a name here
        using nv_sensor_type = std::remove_reference_t<decltype(nv_thermal_settings.sensor[0])>;

        nv_sensor_type* const sensors_begin = nv_thermal_settings.sensor;
        nv_sensor_type* const sensors_end   = nv_thermal_settings.sensor + sensors_count;

        // for each sensor, create a thermal_sensor_info and place at the end of the vector
        std::transform(sensors_begin, sensors_end, std::back_inserter(sensor_infos), [](const nv_sensor_type& nv_sensor) -> thermal_sensor_info {
          thermal_sensor_info sensor_info;
          sensor_info.current_temp = nv_sensor.currentTemp;
          sensor_info.sensor_type  = _nv_thermal_target_convert(nv_sensor.target);

          return sensor_info;
        });

        return sensor_infos;
      }

    private:
      /// The handle to a physical device provided by NVAPI
      NvPhysicalGpuHandle _gpu_handle;
    };

    ///
    /// Append discovered NVIDIA devices to the provided container
    /// \internal
    ///
    void _nv_fill_devices(std::vector<std::shared_ptr<const device>>& devices_container) {
      NvPhysicalGpuHandle physical_handles[NVAPI_MAX_PHYSICAL_GPUS];
      NvU32 physical_handles_count;

      NvAPI_Status status = NVAPI_OK;

      status = NvAPI_EnumPhysicalGPUs(physical_handles, std::addressof(physical_handles_count));
      if (status != NVAPI_OK) {
        // TODO: handle error
        // Note that it is possible for no GPUs to be found (which returns not OK status)
        assert(false);
      }

      // construct an nvidia_device for each handle that we got
      // and make sure to reserve enough room ahead of time
      devices_container.reserve(devices_container.size() + std::size_t{ physical_handles_count });
      for (NvU32 i = 0; i < physical_handles_count; ++i) {
        devices_container.emplace_back(std::make_shared<nvidia_device>(physical_handles[i]));
      }
    }
  }

  const std::vector<std::shared_ptr<const device>>& devices() {
    static const auto devices = std::invoke([]() -> std::vector<std::shared_ptr<const device>> {
      std::vector<std::shared_ptr<const device>> devices;
      // fill with nvidia devices
      _nv_fill_devices(devices);
      return devices;
    });
    
    // devices is initialized once in a thread-safe manner
    return devices;
  }

  namespace iostream {
    std::ostream& operator<<(std::ostream& os, const memory_info& info) {
      os << "[";
      os << "(dedicated=" << info.dedicated << "kB)";
      os << ",";
      os << "(available_dedicated=" << info.available_dedicated << "kB)";
      os << ",";
      os << "(system=" << info.system << "kB)";
      os << ",";
      os << "(shared_system=" << info.shared_system << "kB)";
      os << "]";

      return os;
    }

    std::ostream& operator<<(std::ostream& os, const thermal_sensor_info& info) {
      os << "[";
      os << "(current_temp=" << info.current_temp << "C)";
      os << ",";
      os << "(sensor_type=" << thermal_sensor_type_to_string(info.sensor_type) << ")";
      os << "]";

      return os;
    }
  }
}
