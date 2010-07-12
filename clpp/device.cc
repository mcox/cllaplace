#include <CL/cl.h>
#include "platform_internal.hh"
#include "device.hh"
#include "device_internal.hh"
#include "error.hh"

cl::device::impl::impl(const cl_device_id id)
  : did(id)
{
}

cl::device::impl::impl(const impl &i)
  : did(i.did)
{
}

cl::device::impl &cl::device::impl::operator=(const impl &i)
{
  did = i.did;
}

cl::device::impl::~impl(void)
{
}

cl_device_id cl::device::impl::get_device(void) const
{
  return did;
}

std::string cl::device::impl::get_device_string(cl_device_info param_name) const
{
  /* Get string length */
  std::size_t param_len;
  cl_int cl_err = clGetDeviceInfo(did, param_name, 0, NULL, &param_len);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to determine device parameter length");
  }
  /* Get string */
  std::vector<char> param_val(param_len);
  cl_err = clGetDeviceInfo(did, param_name, param_val.size(), &param_val[0],
      &param_len);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to acquire device parameter string");
  }
  if(param_val.size() != param_len) {
    throw cl::error("device parameter length mismatch");
  }

  return std::string(param_val.begin(), param_val.end());
}

cl::device::device(const impl &i)
  : pimpl(new impl(i))
{
}

std::vector<cl::device> cl::device::get_devices(type t)
{
  return impl::get_devices(NULL, t);
}

std::vector<cl::device> cl::device::get_devices(platform p, type t)
{
  return impl::get_devices(p.pimpl->pid, t);
}

cl::device::device(const device &dev)
  : pimpl(new impl(*dev.pimpl))
{
}

cl::device::~device(void)
{
}

cl::device &cl::device::operator=(const device &rhs)
{
  *pimpl = *rhs.pimpl;

  return *this;
}

std::string cl::device::name(void) const
{
  return pimpl->get_device_string(CL_DEVICE_NAME);
}

std::string cl::device::driver_version(void) const
{
  return pimpl->get_device_string(CL_DRIVER_VERSION);
}

