#ifndef CLPP_CL_EVENT_INTERNAL_HH_INCLUDED
#define CLPP_CL_EVENT_INTERNAL_HH_INCLUDED

#include <CL/cl.h>
#include "event.hh"

class cl::event::impl {
  private:
  cl_event ev;

  public:
  impl(cl_event e, bool retain = true);
  impl(const impl &i);
  impl &operator=(const impl &i);
  ~impl(void);

  cl_event get_event(void) const;
  cl_int event_status(void) const;

  // Note that the returned cl_events are not retained!
  static std::vector<cl_event> get_events(const std::vector<event> &evs);
};

#endif /* CLPP_CL_EVENT_INTERNAL_HH_INCLUDED */
