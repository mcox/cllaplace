#include <cmath>
#include <iostream>
#include <fstream>
#include <boost/timer.hpp>
#include "clpp/clpp.hh"
#include "laplace.hh"
#include "defaults.hh"

// Obtain standard sized integers
#include <stdint.h>

// Grab the param_t structure
#define HOST_INCLUSION
#include "laplace_jac.cl"

help_activated::help_activated(void)
  : runtime_error("help activated")
{
}

cl::platform select_platform(void)
{
  std::vector<cl::platform> platforms(cl::platform::get_platforms());
  if(platforms.size() == 0) {
    throw std::runtime_error("no available platforms");
  }
  /* For now, just pick the first platform */
  cl::platform platform = platforms[0];
  if(defaults::get().verbose()) {
    std::cerr << "Selected platform '" << platform.name() << "' version " <<
      platform.version() << std::endl;
  }
  return platform;
}

cl::device select_device(cl::platform platform)
{
  cl::device::type dtype = defaults::get().dev_type();
  std::vector<cl::device> devices(cl::device::get_devices(platform, dtype));
  if(devices.size() == 0) {
    throw std::runtime_error("no available devices");
  }
  /* For now, just pick the first device */
  cl::device device = devices[0];
  if(defaults::get().verbose()) {
    std::cerr << "Selected device '" << device.name() << "' driver version " <<
      device.driver_version() << std::endl;
  }
  return device;
}

params_t make_params(void)
{
  params_t ret;
  
  ret.global_dims[0] = defaults::get().lattice_size().dim[0];
  ret.global_dims[1] = defaults::get().lattice_size().dim[1];
  ret.global_row_stride = defaults::get().lattice_size().dim[0];
  ret.xmin = ret.ymin = 0.0f;
  ret.xmax = ret.ymax = M_PI;

  return ret;
}

void write_data(const params_t &params, const std::vector<float> &dat)
{
  std::ofstream fout("laplace.out");
  std::vector<float> output;
  output.reserve(params.global_dims[1] + 1);
  // Write data in the binary format supported by gnuplot. Note the stupid
  // FORTRAN-column major format.
  // <N+1><y0><y1><y2>...<yN>      header row
  // <x0><z00><z10><z20>..<zN0>    first column
  // <x1><z01><z11><z21>..<zN1>    second column
  // <x2><z02><x12><x22>..<zN2>    third column, etc.
  // ..........................
  // <xM><z0M><z1M><z2M>..<zNM>
  output.push_back(params.global_dims[1]);
  for(unsigned r = 0; r < params.global_dims[1]; ++r)
  {
    output.push_back(params.ymin +
        static_cast<float>(r)/params.global_dims[1]*
        (params.ymax - params.ymin));
  }
  fout.write((char *)(&output[0]), output.size()*sizeof(output[0]));

  for(unsigned c = 0; fout && c < params.global_dims[0]; ++c) {
    output.clear(); output.reserve(params.global_dims[1] + 1);
    output.push_back(params.xmin +
        static_cast<float>(c)/params.global_dims[0]*
        (params.xmax - params.xmin));

    for(unsigned r = 0; r < params.global_dims[1]; ++r) {
      output.push_back(dat.at(r*params.global_row_stride + c));
    }

    fout.write((char *)(&output[0]), output.size()*sizeof(output[0]));
  }
}

int main(int argc, char **argv)
try {
  defaults::process_arguments(argc, argv);

  if(defaults::get().verbose()) {
    std::cerr << "Selected device type '" << defaults::get().dev_type() <<
      '\'' << std::endl;
  }

  /* Setup params */
  params_t param_val = make_params();
  /* Allocate space to recieve state back (for printing) */
  std::vector<float> data(param_val.global_row_stride*param_val.global_dims[1]);

  cl::platform platform = select_platform();
  cl::device device = select_device(platform);
  cl::context context = cl::context::create(device);
  cl::queue queue = cl::queue::create(context, device);
  cl::program program = cl::program::from_source(context, "laplace_jac.cl");
  cl::program::build_info build = program.build(device);

  if(!build) {
    std::cerr << "Build failed, log follows:" << std::endl;
  } else if(defaults::get().verbose()) {
    std::cerr << "Build succeeded, log follows:" << std::endl;
  }
  if(!build || defaults::get().verbose()) {
    std::cerr << build.build_log() << std::endl;
  }

  cl::kernel init_kernel = program.get_kernel("init_domain");
  cl::kernel jac_kernel = program.get_kernel("jacobi_step");
  cl::buffer params = cl::buffer::create(context, sizeof(params_t), cl::mem::MEM_MODE_RO);
  queue.add(cl::buffer_write(params, &param_val, sizeof(param_val)));
  init_kernel.argv()[0] <<= params;
  jac_kernel.argv()[0] <<= params;
  cl::buffer state = cl::buffer::create(context,
      defaults::get().lattice_size().dim[1]*param_val.global_row_stride*sizeof(float));
  init_kernel.argv()[1] <<= state;
  jac_kernel.argv()[1] <<= state;
  cl::buffer diffs = cl::buffer::create(context,
      defaults::get().lattice_size().dim[1]*param_val.global_row_stride*sizeof(float));
  init_kernel.argv()[2] <<= diffs;
  jac_kernel.argv()[2] <<= diffs;
  // Figure out local space req'd (two additional rows and columns) and add
  // local space to the jacobian kernel.
  jac_kernel.argv()[3] <<= cl::local_space(sizeof(float)*
      (defaults::get().local_size().dim[0] + 2)*
      (defaults::get().local_size().dim[1] + 2));
  // Setup kernel execution and initialize state (on device)
  cl::nd_run run_init(init_kernel, defaults::get().lattice_size().dim,
      defaults::get().local_size().dim);
  cl::nd_run run_jacobi(jac_kernel, defaults::get().lattice_size().dim,
      defaults::get().local_size().dim);
  cl::event ev = queue.add(run_init);
  ev.wait();
  if(defaults::get().verbose()) {
    std::cerr << "Initialization finished, started timing" << std::endl;
  }
  boost::timer runtime;
  for(int i = 0; i < 10000; ++i) {
    ev = queue.add(run_jacobi, std::vector<cl::event>(1,ev));
  }
  ev.wait();
  if(defaults::get().verbose()) {
    std::cerr << "Run finished, elapsed time: " << runtime.elapsed() << std::endl;
  }

  // Setup to recieve state back from the device
  queue.add(cl::buffer_read(state, &data[0], data.size()*sizeof(data[0]))).wait();

  // Write data back to file
  write_data(param_val, data);

  return 0;
} catch(help_activated &help) {
  return 0;
} catch(cl::error &cl_err) {
  std::cerr << "Terminating due to OpenCL exception: " << cl_err.what() <<
    std::endl;
} catch(std::exception &e) {
  std::cerr << "Terminating due to exception: " << e.what() << std::endl;
} catch(...) {
  std::cerr << "Uknown exception. Terminating program." << std::endl;
  return 1;
}
