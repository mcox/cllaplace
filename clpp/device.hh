#ifndef CLPP_CL_DEVICE_HH_INCLUDED
#define CLPP_CL_DEVICE_HH_INCLUDED

#include <memory>
#include <locale>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "platform.hh"

namespace cl {
  class context;
  class program;
  class queue;

  class device {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit device(const impl &i);

    public:
    enum type {
      CPU, GPU, ACCELERATOR, DEFAULT, ALL
    };

    //! Obtain available devices
    /*! Note that the behaviour of this function (the variant without a
     *  platform parameter) is implementation defined. For portability, you
     *  should avoid this function and pass an explicit platform instead.
     */
    static std::vector<device> get_devices(type t);
    //! Obtain available devices for a given platform
    static std::vector<device> get_devices(cl::platform p, type t);

    device(const device &dev);
    ~device(void);

    device &operator=(const device &rhs);

    std::string name(void) const;
    std::string driver_version(void) const;

    friend class context;
    friend class program;
    friend class queue;
  };
}

#include <iostream>

template<typename CharT, typename Traits>
std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits> &is, cl::device::type &t)
{
  /* Eat up white space */
  is >> std::ws;
  std::basic_string<CharT, Traits> s;
  is >> s;

  if(s.compare("CPU") == 0) {
    t = cl::device::CPU;
  } else if(s.compare("GPU") == 0) {
    t = cl::device::GPU;
  } else if(s.compare("ACCELERATOR") == 0) {
    t = cl::device::ACCELERATOR;
  } else if(s.compare("DEFAULT") == 0) {
    t = cl::device::DEFAULT;
  } else if(s.compare("ALL") == 0) {
    t = cl::device::ALL;
  } else {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits> &os, cl::device::type t)
{
  switch(t) {
    case cl::device::GPU:
      os << "GPU";
      break;
    case cl::device::CPU:
      os << "CPU";
      break;
    case cl::device::ACCELERATOR:
      os << "ACCELERATOR";
      break;
    case cl::device::DEFAULT:
      os << "DEFAULT";
      break;
    case cl::device::ALL:
      os << "ALL";
      break;
  }
  return os;
}

#endif /* CLPP_CL_DEVICE_HH_INCLUDED */
