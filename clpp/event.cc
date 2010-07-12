#include <iostream>
#include "error.hh"
#include "event_internal.hh"

cl::event::impl::impl(cl_event e, bool retain)
  : ev(e)
{
  if(retain) {
    cl_int cl_err = clRetainEvent(ev);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to retain event");
    }
  }
}

cl::event::impl::impl(const impl &i)
  : ev(i.ev)
{
  cl_int cl_err = clRetainEvent(ev);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain event");
  }
}

cl::event::impl &cl::event::impl::operator=(const impl &i)
{
  cl_int cl_err = clRetainEvent(i.ev);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain event");
  }

  cl_err = clReleaseEvent(ev);
  if(cl_err != CL_SUCCESS) {
    if(clReleaseEvent(i.ev) != CL_SUCCESS) {
      std::cerr << "unable to release retained event" << std::endl;
    }
    throw cl::error("unable to release event");
  }

  ev = i.ev;

  return *this;
}

cl::event::impl::~impl(void)
{
}

cl_event cl::event::impl::get_event(void) const
{
  return ev;
}

cl_int cl::event::impl::event_status(void) const
{
  cl_int stat = 0;
  cl_int cl_err = clGetEventInfo(ev, CL_EVENT_COMMAND_EXECUTION_STATUS,
      sizeof(stat), &stat, NULL);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to determine event execution status");
  }

  return stat;
}

cl::event::event(const impl &i)
  : pimpl(new impl(i))
{
}

cl::event::event(const event &e)
  : pimpl(new impl(*e.pimpl))
{
}

cl::event &cl::event::operator=(const event &e)
{
  *pimpl = *e.pimpl;

  return *this;
}

cl::event::~event(void)
{
}

void cl::event::wait(void) const
{
  const cl_event ev = pimpl->get_event();

  cl_int cl_err = clWaitForEvents(1, &ev);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to wait on event");
  }
}

void cl::event::wait_all(const std::vector<event> &evs)
{
  const std::vector<cl_event> events = impl::get_events(evs);

  int cl_err = clWaitForEvents(events.size(), &events[0]);

  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to wait for events");
  }
}

bool cl::event::completed(void) const
{
  return pimpl->event_status() == CL_COMPLETE;
}

std::vector<cl_event>
cl::event::impl::get_events(const std::vector<event> &evs)
{
  std::vector<cl_event> ret;
  ret.reserve(evs.size());

  for(std::vector<event>::const_iterator ev = evs.begin();
      ev != evs.end(); ++ev)
  {
    ret.push_back(ev->pimpl->get_event());
  }

  return ret;
}
