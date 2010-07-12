#ifndef CLPP_CONTEXT_INTERNAL_HH_INCLUDED
#define CLPP_CONTEXT_INTERNAL_HH_INCLUDED

#include <CL/cl.h>
#include "context.hh"

class cl::context::impl {
  private:
  cl_context ctx;

  public:
  /* Call with retain = false when constructing a new context (which is already
   * retained/ref-counted at one), but retain when copying a context.
   */
  explicit impl(cl_context &c, bool retain = true);
  impl(const impl &i);
  ~impl(void) throw();

  impl &operator=(const impl &r);

  cl_context get_context(void) const;

  friend class cl::context;
};

#endif /* CLPP_CONTEXT_INTERNAL_HH_INCLUDED */
