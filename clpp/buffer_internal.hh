#ifndef CLPP_CL_BUFFER_INTERNAL_HH_INCLUDED
#define CLPP_CL_BUFFER_INTERNAL_HH_INCLUDED

#include <CL/cl.h>
#include "buffer.hh"

class cl::mem::impl {
  private:
  cl_mem memory;

  public:
  explicit impl(const cl_mem m, bool retain = true);
  impl(const impl &i);
  ~impl(void);
  impl &operator=(const impl &i);

  cl_mem get_mem(void) const;

  static cl_mem_flags unwrap_flag(const mem_mode m);
};

#endif /* CLPP_CL_BUFFER_INTERNAL_HH_INCLUDED */
