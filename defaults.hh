#ifndef DEFAULT_HH_INCLUDED
#define DEFAULT_HH_INCLUDED

#include "clpp/device.hh"

class defaults {
  private:
  bool verbosity;
  bool synch_ops;
  cl::device::type dtype;

  defaults(void);
  defaults(const defaults &def);
  ~defaults(void);
  
  defaults &operator=(const defaults &def);

  public:
  bool verbose(void) const;
  bool synch(void) const;
  cl::device::type dev_type(void) const;
  struct area {
    std::size_t dim[2];
  };
  area lattice_size(void) const;
  area local_size(void) const;

  static defaults &get(void);
  static void process_arguments(int argc, char **argv);
};

#endif /* DEFAULT_HH_INCLUDED */
