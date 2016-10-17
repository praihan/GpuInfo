#include <GpuInfo.h>

#include <Windows.h>
#include <nvapi.h>

#include <utility>
#include <functional>
#include <memory>
#include <cassert>

namespace gpuinfo {

  struct nvidia_device : device {
    nvidia_device(NvPhysicalGpuHandle gpu_handle) : _gpu_handle(gpu_handle) {
      // TODO: handle error
      assert(this->_gpu_handle != nullptr);
    }

    virtual std::string name() const override {
      NvAPI_ShortString name_string;
      NvAPI_Status status = NVAPI_OK;

      status = NvAPI_GPU_GetFullName(this->_gpu_handle, name_string);
      if (status != NVAPI_OK) {
        // TODO: handle error
      }

      return std::string{ name_string };
    }

    virtual memory_info memory() const override {
      return memory_info();
    }
    virtual std::vector<thermal_sensor_info> thermal_sensors() const override {
      return std::vector<thermal_sensor_info>();
    }

    NvPhysicalGpuHandle _gpu_handle;
  };

  const std::vector<std::shared_ptr<const device>>& devices() {
    static const auto devices = std::invoke([]() -> std::vector<std::shared_ptr<const device>> {

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
      std::vector<std::shared_ptr<const device>> devices;
      for (NvU32 i = 0; i < physical_handles_count; ++i) {
        devices.emplace_back(std::make_shared<nvidia_device>(physical_handles[i]));
      }

      return devices;

    });
    
    // devices is initialized once in a thread-safe manner
    return devices;
  }
}
