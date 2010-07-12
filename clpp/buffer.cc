#include <iostream>

#include "error.hh"
#include "buffer_internal.hh"
#include "context_internal.hh"

cl::mem::impl::impl(const cl_mem m, bool retain)
  : memory(m)
{
  if(retain) {
    int cl_err = clRetainMemObject(memory);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to retain memory object");
    }
  }
}

cl::mem::impl::impl(const impl &i)
  : memory(i.memory)
{
  int cl_err = clRetainMemObject(memory);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain memory object");
  }
}

cl::mem::impl::~impl(void)
{
  int cl_err = clReleaseMemObject(memory);
  if(cl_err != CL_SUCCESS) {
    std::cout << "unable to release memory object in cl::mem::impl::~impl"
      << std::endl;
  }
}

cl::mem::impl &cl::mem::impl::operator=(const impl &i)
{
  int cl_err = clRetainMemObject(i.memory);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain memory object");
  }

  cl_err = clReleaseMemObject(memory);
  if(cl_err != CL_SUCCESS) {
    if(clReleaseMemObject(i.memory) != CL_SUCCESS) {
      std::cout << "unable to release retained memory object" << std::endl;
    }
    throw cl::error("unable to release memory object");
  }

  memory = i.memory;
  return *this;
}

cl_mem cl::mem::impl::get_mem(void) const
{
  return memory;
}

cl_mem_flags cl::mem::impl::unwrap_flag(const mem_mode m)
{
  switch(m) {
    case MEM_MODE_RW:
      return CL_MEM_READ_WRITE;
    case MEM_MODE_RO:
      return CL_MEM_READ_ONLY;
    case MEM_MODE_WO:
      return CL_MEM_WRITE_ONLY;
    default:
      throw cl::error("unknown memory flag");
  }
}

cl::mem::mem(const impl &i)
  : pimpl(new impl(i))
{
}

cl::mem::mem(const mem &m)
  : pimpl(new impl(*m.pimpl))
{
}

cl::mem &cl::mem::operator=(const mem &m)
{
  *pimpl = *m.pimpl;

  return *this;
}

cl::mem::~mem(void)
{
}

cl::buffer::buffer(const impl &i)
  : mem(i)
{
}

cl::buffer::buffer(const buffer &b)
  : mem(*b.pimpl)
{
}

cl::buffer::~buffer(void)
{
}

cl::buffer cl::buffer::create(const context &c, std::size_t cb, mem_mode mode)
{
  const cl_context ctx = c.pimpl->get_context();
  const cl_mem_flags mflag = impl::unwrap_flag(mode);

  cl_int cl_err = CL_SUCCESS;
  cl_mem m = clCreateBuffer(ctx, mflag, cb, NULL, &cl_err);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to create buffer");
  }

  return buffer(impl(m, false));
}

void cl::buffer::swap(buffer &b)
{
  std::swap(pimpl, b.pimpl);
}

cl::local_space::local_space(std::size_t s)
  : cb(s)
{
}

cl::local_space::local_space(const local_space &ls)
  : cb(ls.cb)
{
}

cl::local_space &cl::local_space::operator=(const local_space &ls)
{
  cb = ls.cb;
}

cl::local_space::~local_space(void)
{
}

void cl::local_space::swap(cl::local_space &ls)
{
  std::swap(cb, ls.cb);
}

std::size_t cl::local_space::size(void) const
{
  return cb;
}

template<> void std::swap(cl::buffer &a, cl::buffer &b)
{
  a.swap(b);
}

template<> void std::swap(cl::local_space &a, cl::local_space &b)
{
  a.swap(b);
}
