#ifndef CLPP_CL_QUEUE_INTERNAL_HH_INCLUDED
#define CLPP_CL_QUEUE_INTERNAL_HH_INCLUDED

#include <CL/cl.h>
#include "queue.hh"

class cl::queue::impl {
  private:
  cl_command_queue command_queue;

  public:
  impl(const cl_command_queue q, bool retain = true);
  impl(const impl &i);
  ~impl(void);
  impl &operator=(const impl &i);

  cl_command_queue get_command_queue(void) const;
};

#endif /* CLPP_CL_QUEUE_INTERNAL_HH_INCLUDED */
