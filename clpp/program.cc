#include <iostream>
#include <fstream>
#include <vector>

#include "error.hh"
#include "program_internal.hh"
#include "context_internal.hh"
#include "device_internal.hh"
#include "buffer_internal.hh"

cl::kernel::impl::impl(cl_kernel k, bool retain)
  : kern(k)
{
  if(retain) {
    int cl_err = clRetainKernel(kern);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to retain kernel");
    }
  }
}

cl::kernel::impl::impl(const impl &i)
  : kern(i.kern)
{
  int cl_err = clRetainKernel(kern);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain kernel");
  }
}

cl::kernel::impl &cl::kernel::impl::operator=(const impl &i)
{
  int cl_err = clRetainKernel(i.kern);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain kernel");
  }

  cl_err = clReleaseKernel(kern);
  if(cl_err != CL_SUCCESS) {
    if(clReleaseKernel(i.kern) != CL_SUCCESS) {
      std::cerr << "unable to release retained kernel" << std::endl;
    }
    throw cl::error("unable to release kernel");
  }

  kern = i.kern;

  return *this;
}

cl::kernel::impl::~impl(void)
{
  int cl_err = clReleaseKernel(kern);
  if(cl_err != CL_SUCCESS) {
    std::cerr << "unable to release kernel in cl::kernel::impl::~impl" <<
      std::endl;
  }
}

cl_kernel cl::kernel::impl::get_kernel(void) const
{
  return kern;
}

cl::program::impl::impl(cl_program p, bool retain)
  : prog(p)
{
  if(retain) {
    int cl_err = clRetainProgram(prog);
    if(cl_err != CL_SUCCESS) {
      throw cl::error("unable to retain program");
    }
  }
}

cl::program::impl::impl(const impl &i)
  : prog(i.prog)
{
  int cl_err = clRetainProgram(prog);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain program");
  }
}

cl::program::impl::~impl(void) throw()
{
  int cl_err = clReleaseProgram(prog);
  if(cl_err != CL_SUCCESS) {
    std::cerr << "unable to release program in cl::program::impl::~impl"
      << std::endl;
  }
}

cl::program::impl &cl::program::impl::operator=(const impl &i)
{
  int cl_err = clRetainProgram(i.prog);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to retain program");
  }

  cl_err = clReleaseProgram(prog);
  if(cl_err != CL_SUCCESS) {
    if(clReleaseProgram(i.prog) != CL_SUCCESS) {
      std::cerr << "unable to release retained program" << std::endl;
    }
    throw cl::error("unable to release program");
  }

  prog = i.prog;

  return *this;
}

cl_program cl::program::impl::get_program(void) const
{
  return prog;
}

cl::program::program(const impl &i)
  : pimpl(new impl(i))
{
}

cl::program::program(const program &p)
  : pimpl(new impl(*p.pimpl))
{
}

cl::program::~program(void)
{
}

cl::program &cl::program::operator=(const program &p)
{
  *pimpl = *p.pimpl;

  return *this;
}

cl::program cl::program::from_source(const context &c,
    const std::string &filename)
{
  /* Read the source in chunks */
  std::ifstream source(filename.c_str());
  std::string data;
  std::vector<char> read_buf(1024);
  std::size_t read_size;

  while(source.get(&read_buf[0], read_buf.size(),
        std::ifstream::traits_type::eof())) {
    // TODO: use a non-O(n^2) algorithm for this
    data.append(&read_buf[0], source.gcount());
  }

  /* Create the program */
  const char *data_chunk = data.data();
  std::size_t chunk_len = data.length();
  cl_int cl_err = CL_SUCCESS;
  cl_program prog =clCreateProgramWithSource(c.pimpl->get_context(), 1,
      &data_chunk, &chunk_len, &cl_err);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("error creating program");
  }

  return program(impl(prog, false));
}

cl::program::build_info cl::program::build(const device &dev) const
{
  const cl_device_id did = dev.pimpl->get_device();
  cl_int cl_err = clBuildProgram(pimpl->get_program(), 1, &did, NULL, NULL,
      NULL);
  if(cl_err != CL_SUCCESS && cl_err != CL_BUILD_PROGRAM_FAILURE) {
    throw cl::error("error building program");
  }

  return build_info(*this, dev);
}

cl::kernel cl::program::get_kernel(const std::string &name) const
{
  const char *cname = name.c_str();
  const cl_program p = pimpl->get_program();

  cl_int cl_err = CL_SUCCESS;
  const cl_kernel k = clCreateKernel(p, cname, &cl_err);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to obtain kernel");
  }

  // TODO: At this point, we have responsibility to delete the kernel if we
  // screw the pooch herein. Fix this. Similar bugs exist in context, queue,
  // program, event, buffer.
  return kernel(kernel::impl(k, false));
}

cl::program::build_info::build_info(const program &p, const device &d)
  : prog(p)
  , dev(d)
{
}

cl::program::build_info::build_info(const build_info &bi)
  : prog(bi.prog)
  , dev(bi.dev)
{
}

cl::program::build_info::~build_info(void)
{
}

std::string cl::program::build_info::build_log(void) const
{
  cl_program p = prog.pimpl->get_program();
  cl_device_id d = dev.pimpl->get_device();

  /* Determine build log length */
  std::size_t log_len = 0;
  cl_int cl_err = clGetProgramBuildInfo(p, d, CL_PROGRAM_BUILD_LOG,
      0, NULL, &log_len);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to determine program build log length");
  }

  /* Get build log */
  std::vector<char> log(log_len);
  cl_err = clGetProgramBuildInfo(p, d, CL_PROGRAM_BUILD_LOG,
      log.size(), &log[0], &log_len);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to obtain program build log");
  }
  if(log.size() != log_len) {
    throw cl::error("program build log length mismatch");
  }

  return std::string(log.begin(), log.end());
}

bool cl::program::build_info::operator!(void) const
{
  cl_program p = prog.pimpl->get_program();
  cl_device_id d = dev.pimpl->get_device();

  cl_build_status build_status = CL_BUILD_NONE;
  cl_int cl_err = clGetProgramBuildInfo(p, d, CL_PROGRAM_BUILD_STATUS,
      sizeof(build_status), &build_status, NULL);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to obtain build status");
  }

  return build_status != CL_BUILD_SUCCESS;
}

cl::program::build_info &cl::program::build_info::operator=(
    const build_info &r)
{
  // TODO: Offer strong exception guarantee by copying to a new value and then
  // performing no-throw swap
  prog = r.prog;
  dev = r.dev;

  return *this;
}

cl::kernel::kernel(const impl &i)
  : pimpl(new impl(i))
{
}

cl::kernel::kernel(const kernel &k)
  : pimpl(new impl(*k.pimpl))
{
}

cl::kernel &cl::kernel::operator=(const kernel &k)
{
  *pimpl = *k.pimpl;

  return *this;
}

cl::kernel::~kernel(void)
{
}

std::vector<cl::kernel::arg_proxy> cl::kernel::argv(void)
{
  const unsigned int nargs = argc();
  std::vector<arg_proxy> args;
  args.reserve(nargs);

  for(unsigned i = 0; i < nargs; ++i) {
    args.push_back(arg_proxy(*this, i));
  }

  return args;
}

unsigned int cl::kernel::argc(void) const
{
  const cl_kernel kobj = pimpl->get_kernel();
  cl_uint nargs = 0;
  cl_int cl_err = clGetKernelInfo(kobj, CL_KERNEL_NUM_ARGS, sizeof(nargs),
      &nargs, NULL);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to obtain kernel argument count");
  }

  return nargs;
}

void cl::kernel::swap(kernel &k)
{
  std::swap(pimpl, k.pimpl);
}

cl::kernel::arg_proxy::arg_proxy(const kernel &k, unsigned int n)
  : kern(k), argnum(n)
{
}

cl::kernel::arg_proxy::arg_proxy(const arg_proxy &ap)
  : kern(ap.kern), argnum(ap.argnum)
{
}

cl::kernel::arg_proxy &cl::kernel::arg_proxy::operator=(const arg_proxy &ap)
{
  kern = ap.kern;
  argnum = ap.argnum;
}

cl::kernel::arg_proxy::~arg_proxy(void)
{
}

void cl::kernel::arg_proxy::operator<<=(const mem &m)
{
  const cl_mem memobj = m.pimpl->get_mem();
  const cl_kernel kobj = kern.pimpl->get_kernel();

  cl_int cl_err = clSetKernelArg(kobj, argnum, sizeof(cl_mem), &memobj);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to bind kernel argument");
  }
}

void cl::kernel::arg_proxy::operator<<=(const local_space &ls)
{
  const cl_kernel kobj = kern.pimpl->get_kernel();

  cl_int cl_err = clSetKernelArg(kobj, argnum, ls.size(), NULL);
  if(cl_err != CL_SUCCESS) {
    throw cl::error("unable to bind local memory");
  }
}

template<>
void std::swap(cl::kernel &a, cl::kernel &b)
{
  a.swap(b);
}
