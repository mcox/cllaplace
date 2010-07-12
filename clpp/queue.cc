#include <iostream>

#include "error.hh"
#include "device_internal.hh"
#include "event_internal.hh"
#include "context_internal.hh"
#include "queue_internal.hh"
#include "program_internal.hh"
#include "buffer_internal.hh"

cl::queue::impl::impl(const cl_command_queue q, bool retain)
  : command_queue(q)
{
  if(retain) {
    cl_int cl_err = clRetainCommandQueue(command_queue);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to retain command queue");
    }
  }
}

cl::queue::impl::impl(const impl &i)
  : command_queue(i.command_queue)
{
  cl_int cl_err = clRetainCommandQueue(command_queue);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain command queue");
  }
}

cl::queue::impl::~impl(void)
{
  cl_int cl_err = clReleaseCommandQueue(command_queue);
  if(cl_err != CL_SUCCESS) {
    std::cerr << "unable to release command queue in cl::queue::impl::~impl" <<
      std::endl;
  }
}

cl::queue::impl &cl::queue::impl::operator=(const impl &i)
{
  cl_int cl_err = clRetainCommandQueue(i.command_queue);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain command queue");
  }

  cl_err = clReleaseCommandQueue(command_queue);
  if(cl_err != CL_SUCCESS) {
    // TODO: Replace this with a cleaner sentinel class which is disarmed if the
    // release succeeds.
    if(clReleaseCommandQueue(i.command_queue) != CL_SUCCESS) {
      std::cerr << "unable to release retained command queue" << std::endl;
    }
    throw cl::error("unable to release command queue");
  }

  command_queue = i.command_queue;

  return *this;
}

cl_command_queue cl::queue::impl::get_command_queue(void) const
{
  return command_queue;
}

cl::queue::queue(const impl &i)
  : pimpl(new impl(i))
{
}

cl::queue::queue(const queue &q)
  : pimpl(new impl(*q.pimpl))
{
}

cl::queue::~queue(void)
{
}

cl::queue &cl::queue::operator=(const queue &q)
{
  *pimpl = *q.pimpl;

  return *this;
}

cl::queue cl::queue::create(const context &c, const device &d)
{
  cl_int cl_err = CL_SUCCESS;
  const cl_device_id dev = d.pimpl->get_device();
  const cl_context ctx = c.pimpl->get_context();
  cl_command_queue q = clCreateCommandQueue(ctx, dev,
      CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &cl_err);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to create command queue");
  }

  return queue(impl(q, false));
}

cl::event cl::queue::add(const nd_run &nd, const std::vector<event> &waitlist)
{
  std::vector<cl_event> waitevs = event::impl::get_events(waitlist);

  cl_event ev;
  int cl_err = CL_SUCCESS;
  cl_err = clEnqueueNDRangeKernel(
      pimpl->get_command_queue(), // Get the command queue primitive
      nd.k.pimpl->get_kernel(), // Get the kernel primitive
      nd.wd, // workspace dimension
      NULL, // always null in OpenCL 1.0
      &nd.gd[0], // global dimensions
      nd.ld.size() ? &nd.ld[0] : NULL, // local dimensions, null if not spec'd
      waitevs.size(), waitevs.size() ? &waitevs[0] : NULL,
      &ev);

  return event(event::impl(ev, false));
}

cl::event cl::queue::add(const buffer_read &br,
    const std::vector<event> &waitlist, bool blocking)
{
  std::vector<cl_event> waitevs = event::impl::get_events(waitlist);

  cl_event ev;
  int cl_err = CL_SUCCESS;

  cl_err = clEnqueueReadBuffer(pimpl->get_command_queue(),
      br.buf.pimpl->get_mem(), blocking ? CL_TRUE : CL_FALSE,
      br.offsetbytes, br.bytes, br.dst, waitevs.size(),
      waitevs.size() ? &waitevs[0] : NULL, &ev);

  return event(event::impl(ev, false));
}

cl::event cl::queue::add(const buffer_write &bw,
    const std::vector<event> &waitlist, bool blocking)
{
  std::vector<cl_event> waitevs = event::impl::get_events(waitlist);

  cl_event ev;
  int cl_err = CL_SUCCESS;

  cl_err = clEnqueueWriteBuffer(pimpl->get_command_queue(),
      bw.buf.pimpl->get_mem(), blocking ? CL_TRUE : CL_FALSE,
      bw.offsetbytes, bw.bytes, bw.src, waitevs.size(),
      waitevs.size() ? &waitevs[0] : NULL, &ev);

  return event(event::impl(ev, false));
}

cl::nd_run::nd_run(const kernel &kern, std::size_t global_dims[2],
    std::size_t local_dims[2])
  : k(kern), wd(2), gd(global_dims, global_dims + 2), ld()
{
  if(local_dims != 0) {
    ld = std::vector<std::size_t>(local_dims, local_dims + 2);
  }
}

cl::nd_run::nd_run(const nd_run &nds)
  : k(nds.k), wd(nds.wd), gd(nds.gd), ld(nds.ld)
{
}

cl::nd_run &cl::nd_run::operator=(const nd_run &nds)
{
  nd_run newnds(nds);

  swap(newnds);

  return *this;
}

cl::nd_run::~nd_run(void)
{
}

void cl::nd_run::swap(nd_run &nds)
{
  std::swap(k, nds.k);
  std::swap(wd, nds.wd);
  std::swap(gd, nds.gd);
  std::swap(ld, nds.ld);
}

cl::buffer_read::buffer_read(const buffer &b, void *d, std::size_t cb,
    std::size_t os)
  : buf(b), dst(d), bytes(cb), offsetbytes(os)
{
}

cl::buffer_read::buffer_read(const buffer_read &br)
  : buf(br.buf), dst(br.dst), bytes(br.bytes), offsetbytes(br.offsetbytes)
{
}

cl::buffer_read &cl::buffer_read::operator=(const buffer_read &br)
{
  buffer_read newbr(br);

  swap(newbr);

  return *this;
}

cl::buffer_read::~buffer_read(void)
{
}

void cl::buffer_read::swap(buffer_read &br)
{
  std::swap(buf, br.buf);
  std::swap(dst, br.dst);
  std::swap(bytes, br.bytes);
  std::swap(offsetbytes, br.offsetbytes);
}

cl::buffer_write::buffer_write(const buffer &b, void *s, std::size_t cb,
    std::size_t ofs)
  : buf(b), src(s), bytes(cb), offsetbytes(ofs)
{
}

cl::buffer_write::buffer_write(const buffer_write &bw)
  : buf(bw.buf), src(bw.src), bytes(bw.bytes), offsetbytes(bw.offsetbytes)
{
}

cl::buffer_write &cl::buffer_write::operator=(const buffer_write &bw)
{
  buffer_write newbw(bw);

  swap(newbw);

  return *this;
}

cl::buffer_write::~buffer_write(void)
{
}

void cl::buffer_write::swap(buffer_write &bw)
{
  std::swap(buf, bw.buf);
  std::swap(src, bw.src);
  std::swap(bytes, bw.bytes);
  std::swap(offsetbytes, bw.offsetbytes);
}

template<>
void std::swap(cl::nd_run &a, cl::nd_run &b)
{
  a.swap(b);
}

template<>
void std::swap(cl::buffer_read &a, cl::buffer_read &b)
{
  a.swap(b);
}

template<>
void std::swap(cl::buffer_write &a, cl::buffer_write &b)
{
  a.swap(b);
}
