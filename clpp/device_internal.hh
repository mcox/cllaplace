#ifndef CLPP_DEVICE_INTERNAL_HH_INCLUDED
#define CLPP_DEVICE_INTERNAL_HH_INCLUDED

#include <CL/cl.h>
#include "device.hh"
#include "error.hh"

class cl::device::impl {
  private:
  cl_device_id did;

  public:
  explicit impl(const cl_device_id id);
  impl(const impl &i);
  impl &operator=(const impl &i);

  ~impl(void);

  cl_device_id get_device(void) const;

  static cl_device_type unwrap_device_type(const device::type type)
  {
    switch(type) {
      case device::CPU:
        return CL_DEVICE_TYPE_CPU;
      case device::GPU:
        return CL_DEVICE_TYPE_GPU;
      case device::ACCELERATOR:
        return CL_DEVICE_TYPE_ACCELERATOR;
      case device::ALL:
        return CL_DEVICE_TYPE_ALL;
      default:
        throw cl::error("unable to convert unknown device type");
    }
  }

  static device::type rewrap_device_type(const cl_device_type type)
  {
    switch(type) {
      case CL_DEVICE_TYPE_CPU:
        return device::CPU;
      case CL_DEVICE_TYPE_GPU:
        return device::GPU;
      case CL_DEVICE_TYPE_ACCELERATOR:
        return device::ACCELERATOR;
      case CL_DEVICE_TYPE_DEFAULT:
        return device::DEFAULT;
      case CL_DEVICE_TYPE_ALL:
        return device::ALL;
      default:
        throw cl::error("unable to convert unkown device type");
    }
  }

  static std::vector<device> get_devices(cl_platform_id plat, device::type t)
  {
    /* Determine how many devices are available */
    cl_uint num_devices;
    const cl_device_type type = unwrap_device_type(t);
    cl_int cl_err = clGetDeviceIDs(plat, type, 0, NULL, &num_devices);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to determine number of devices");
    }

    /* Get devices */
    std::vector<cl_device_id> dids;
    dids.resize(num_devices);
    cl_err = clGetDeviceIDs(plat, type, dids.size(), &dids[0], &num_devices);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to obtain device IDs");
    }
    if(dids.size() != num_devices) {
      throw cl::error("mismatch in number of devices");
    }

    /* Convert device IDs */
    std::vector<device> devices;
    devices.reserve(num_devices);
    for(std::vector<cl_device_id>::const_iterator did = dids.begin();
        did != dids.end(); ++did) {
      devices.push_back(device(device::impl(*did)));
    }

    return devices;
  }

  std::string get_device_string(cl_device_info param_name) const;
};

#endif /* CLPP_DEVICE_INTERNAL_HH_INCLUDED */
