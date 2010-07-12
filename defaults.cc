#include <iostream>
#include <boost/program_options.hpp>
#include "defaults.hh"
#include "laplace.hh"

defaults::defaults(void)
  : verbosity(false)
  , synch_ops(false)
  , dtype(cl::device::CPU)
{
}

/* copy constructor defaults::defaults(const defaults &) intentionally not
 * defined
 */

defaults::~defaults(void)
{
}

/* assignment operator defaults::operator=(const defaults &) intentionally not
 * defined.
 */

bool defaults::verbose(void) const
{
  return verbosity;
}

bool defaults::synch(void) const
{
  return synch_ops;
}

cl::device::type defaults::dev_type(void) const
{
  return dtype;
}

defaults::area defaults::lattice_size(void) const
{
  area ret;
  ret.dim[0] = 224;
  ret.dim[1] = 224;

  return ret;
}

defaults::area defaults::local_size(void) const
{
  area ret;
  ret.dim[0] = 16;
  ret.dim[1] = 16;

  return ret;
}

defaults &defaults::get(void)
{
  static defaults defs;

  return defs;
}

void defaults::process_arguments(int argc, char **argv)
{
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "produce help message")
      ("verbose,v", "enable verbose output")
      ("synch", "enable synchronous operations")
      ("device,d", po::value<cl::device::type>(&get().dtype),
        "select device type (CPU, GPU)")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if(vm.count("help")) {
    std::cerr << desc << std::endl;
    throw help_activated();
  }

  if(vm.count("verbose")) {
    std::cerr << "Enabling verbose output." << std::endl;
    get().verbosity = true;
  }
  
  if(vm.count("synch")) {
    if(get().verbosity) {
      std::cerr << "Enabling synchronous operation." << std::endl;
    }
    get().synch_ops = true;
  }
}
