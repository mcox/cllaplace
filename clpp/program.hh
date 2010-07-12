#ifndef CLPP_CL_PROGRAM_HH_INCLUDED
#define CLPP_CL_PROGRAM_HH_INCLUDED

#include <algorithm>
#include <memory>
#include <string>
#include "buffer.hh"
#include "context.hh"
#include "device.hh"

namespace cl {
  class queue;
  class program;

  class kernel {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit kernel(const impl &i);
    public:
    kernel(const kernel &k);
    kernel &operator=(const kernel &k);
    ~kernel(void);

    class arg_proxy;
    std::vector<arg_proxy> argv(void);
    unsigned int argc(void) const;

    void swap(kernel &k);

    friend class program;
    friend class queue;
  };

  class kernel::arg_proxy {
    private:
    kernel kern;
    unsigned int argnum;

    arg_proxy(const kernel &k, unsigned int n);
    public:
    arg_proxy(const arg_proxy &ap);
    arg_proxy &operator=(const arg_proxy &ap);
    ~arg_proxy(void);

    //! Assign a buffer or image argument
    void operator<<=(const mem &m);

    //! Assign a local memory area
    void operator<<=(const local_space &ls);

    friend class kernel;
  };

  class program {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit program(const impl &i);

    public:
    program(const program &p);
    ~program(void);

    program &operator=(const program &p);

    //! Create a new program object from source code in a file.
    static program from_source(const context &c,
        const std::string &filename);

    class build_info;
    //! Build a program for a particular device
    build_info build(const device &dev) const;

    //! Obtain a kernel from this program
    kernel get_kernel(const std::string &name) const;

    friend class build_info;
  };

  class program::build_info {
    private:
    program prog;
    device dev;

    public:
    //! Acquire build info for program on a device
    build_info(const program &p, const device &d);
    build_info(const build_info &bi);
    ~build_info(void);
    build_info &operator=(const build_info &bi);

    //! Obtain the build log
    std::string build_log(void) const;

    //! Determine if the build failed
    bool operator!(void) const;
  };
}

namespace std {
  template<>
  void swap(cl::kernel &a, cl::kernel &b);
};

#endif /* CLPP_CL_PROGRAM_HH_INCLUDED */
