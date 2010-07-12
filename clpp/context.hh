#ifndef CLPP_CL_CONTEXT_HH_INCLUDED
#define CLPP_CL_CONTEXT_HH_INCLUDED

#include <memory>
#include "device.hh"

namespace cl {
  class buffer;
  class program;
  class queue;

  class context {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit context(const impl &i);

    public:
    //! Create  a context for a specific device
    static context create(const device &dev);
    // TODO: Add a version handling a vector of devices
    context(const context &ctx);
    ~context(void);

    context &operator=(const context &rhs);

    friend class program;
    friend class queue;
    friend class buffer;
  };
}

#endif /* CLPP_CL_CONTEXT_HH_INCLUDED */
