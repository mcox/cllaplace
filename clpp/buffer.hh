#ifndef CLPP_CL_BUFFER_HH_INCLUDED
#define CLPP_CL_BUFFER_HH_INCLUDED

#include <cstddef>
#include <algorithm>
#include <memory>
#include "event.hh"
#include "context.hh"

namespace cl {
  class queue;
  class kernel;

  class mem {
    protected:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit mem(const impl &i);
    public:
    mem(const mem &m);
    mem &operator=(const mem &m);
    virtual ~mem(void) = 0;

    enum mem_mode {
      MEM_MODE_RW,
      MEM_MODE_RO,
      MEM_MODE_WO
    };

    friend class kernel;
    friend class queue;
  };

  class buffer : public mem {
    private:
    explicit buffer(const impl &i);

    public:
    buffer(const buffer &b);
    ~buffer(void);
    buffer &operator=(const buffer &b);

    //! Create a buffer in a given context with the given size and mode
    static buffer create(const context &c, std::size_t cb,
        mem_mode m = MEM_MODE_RW);

    void swap(buffer &b);
  };

  class local_space {
    private:
    std::size_t cb;

    public:
    explicit local_space(std::size_t s);
    local_space(const local_space &ls);
    local_space &operator=(const local_space &ls);
    ~local_space(void);

    std::size_t size(void) const;

    void swap(local_space &ls);
  };
};

namespace std {
  template<> void swap(cl::buffer &a, cl::buffer &b);
  template<> void swap(cl::local_space &a, cl::local_space &b);
};

#endif /* CLPP_CL_BUFFER_HH_INCLUDED */
