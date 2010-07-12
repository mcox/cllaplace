#ifndef CLPP_PLATFORM_INTERNAL_HH_INCLUDED
#define CLPP_PLATFORM_INTERNAL_HH_INCLUDED

#include <CL/cl.h>

#include "platform.hh"
#include "error.hh"

namespace cl {
  class device;
}

class cl::platform::impl {
  private:
  cl_platform_id pid;

  public:
  explicit impl(const cl_platform_id id);
  impl(const impl &i);
  ~impl(void);

  impl &operator=(const impl &rhs);

  std::string name(void) const;
  std::string version(void) const;

  friend class cl::device;
};

#endif /* CLPP_PLATFORM_INTERNAL_HH_INCLUDED */
