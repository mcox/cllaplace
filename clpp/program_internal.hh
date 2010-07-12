#ifndef CLPP_CL_PROGRAM_INTERNAL_HH_INCLUDED
#define CLPP_CL_PROGRAM_INTERNAL_HH_INCLUDED

#include <CL/cl.h>
#include "program.hh"

class cl::kernel::impl {
  private:
  cl_kernel kern;

  public:
  impl(cl_kernel k, bool retain = true);
  impl(const impl &i);
  impl &operator=(const impl &i);
  ~impl(void);

  cl_kernel get_kernel(void) const;
};

class cl::program::impl {
  private:
  cl_program prog;

  public:
  explicit impl(cl_program p, bool retain = true);
  impl(const impl &i);
  ~impl(void) throw();

  impl &operator=(const impl &rhs);

  cl_program get_program(void) const;
};

#endif /* CLPP_CL_PROGRAM_INTERNAL_HH_INCLUDED */
