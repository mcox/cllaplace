#include <CL/cl.h>

#include "error.hh"
#include "platform.hh"
#include "platform_internal.hh"

cl::platform::impl::impl(const cl_platform_id id)
  : pid(id)
{
}

cl::platform::impl::impl(const impl &i)
  : pid(i.pid)
{
}

cl::platform::impl::~impl(void)
{
}

cl::platform::impl &cl::platform::impl::operator=(const impl &rhs)
{
  pid = rhs.pid;
  
  return *this;
}

namespace cl {
  namespace internal {
    static std::string get_platform_param(cl_platform_id pid,
        cl_platform_info param_name)
    {
      /* Determine param length */
      std::size_t param_size = 0;
      cl_int cl_err = clGetPlatformInfo(pid, param_name, 0, NULL, &param_size);
      if(cl_err != CL_SUCCESS) {
        throw cl::error("error determining platform parameter length");
      }

      /* Get name */
      std::vector<char> param_val(param_size);
      cl_err = clGetPlatformInfo(pid, param_name, param_val.size(),
          &param_val[0], &param_size);
      if(cl_err != CL_SUCCESS) {
        throw cl::error("error determining platform parameter value");
      }
      if(param_size != param_val.size()) {
        throw cl::error("platform parameter length mismatch");
      }

      return std::string(param_val.begin(), param_val.end());
    }
  };
};
std::string cl::platform::impl::name(void) const
{
  return internal::get_platform_param(pid, CL_PLATFORM_NAME);
}

std::string cl::platform::impl::version(void) const
{
  return internal::get_platform_param(pid, CL_PLATFORM_VERSION);
}

cl::platform::platform(const impl &i)
  : pimpl(new impl(i))
{
}

std::vector<cl::platform> cl::platform::get_platforms(void)
{
  /* Determine number of available platforms */
  cl_uint num_platforms = 0;
  cl_int cl_err = clGetPlatformIDs(0, NULL, &num_platforms);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to determine number of platforms");
  }

  /* Get platforms */
  std::vector<cl_platform_id> pids(num_platforms);
  cl_err = clGetPlatformIDs(pids.size(), &pids[0], &num_platforms);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to get platform IDs");
  }
  if(pids.size() != num_platforms) {
    throw cl::error("platform count mismatch");
  }

  std::vector<platform> platforms;
  platforms.reserve(pids.size());
  for(std::vector<cl_platform_id>::const_iterator pid = pids.begin();
      pid != pids.end(); ++pid) {
    platforms.push_back(platform(impl(*pid)));
  }

  return platforms;
}

cl::platform::platform(const platform &p)
  : pimpl(new impl(*p.pimpl))
{
}

cl::platform::~platform(void)
{
}

cl::platform &cl::platform::operator=(const platform &rhs)
{
  *pimpl = *rhs.pimpl;

  return *this;
}

std::string cl::platform::name(void) const
{
  return pimpl->name();
}

std::string cl::platform::version(void) const
{
  return pimpl->version();
}
