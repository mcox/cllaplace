#include <iostream>
#include "error.hh"
#include "device_internal.hh"
#include "context.hh"
#include "context_internal.hh"

cl::context::impl::impl(cl_context &c, bool retain)
  : ctx(c)
{
  if(retain) {
    int cl_err = clRetainContext(ctx);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to retain context");
    }
  }
}

cl::context::impl::impl(const impl &i)
  : ctx(i.ctx)
{
  int cl_err = clRetainContext(ctx);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain context");
  }
}

cl::context::impl::~impl(void) throw()
{
  int cl_err = clReleaseContext(ctx);
  if(cl_err != CL_SUCCESS) {
    std::cerr << "Error releasing context in context::impl::~impl" << std::endl;
  }
}

cl::context::impl &cl::context::impl::operator=(const impl &r)
{
  int cl_err = clRetainContext(r.ctx);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain context");
  }

  cl_err = clReleaseContext(ctx);
  if(cl_err != CL_SUCCESS) {
    if(clReleaseContext(r.ctx) != CL_SUCCESS) {
      std::cerr << "unable to release retained context" << std::endl;
    }
    throw cl::error("unable to release context");
  }

  ctx = r.ctx;

  return *this;
}

cl_context cl::context::impl::get_context(void) const
{
  return ctx;
}

cl::context::context(const impl &i)
  : pimpl(new impl(i))
{
}

cl::context cl::context::create(const device &dev)
{
  cl_int cl_err = CL_SUCCESS;
  const cl_device_id did = dev.pimpl->get_device();
  cl_context ctx = clCreateContext(NULL, 1, &did, NULL, NULL, &cl_err);

  return context(impl(ctx, false));
}

cl::context::context(const context &ctx)
  : pimpl(new impl(*ctx.pimpl))
{
}

cl::context::~context(void)
{
}

cl::context &cl::context::operator=(const context &rhs)
{
  *pimpl = *rhs.pimpl;

  return *this;
}
