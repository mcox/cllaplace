#ifndef CLPP_CL_QUEUE_HEADER_INCLUDED
#define CLPP_CL_QUEUE_HEADER_INCLUDED

#include <algorithm>
#include <memory>
#include "device.hh"
#include "event.hh"
#include "context.hh"
#include "program.hh"

namespace cl {
  class queue;
  class buffer;

  class nd_run {
    private:
    kernel k;
    unsigned wd;
    std::vector<std::size_t> gd, ld;

    public:
    nd_run(const kernel &kern, std::size_t global_dims[2],
        std::size_t local_dims[2] = 0);
    nd_run(const nd_run &nds);
    nd_run &operator=(const nd_run &nds);
    ~nd_run(void);

    void swap(nd_run &nd);

    friend class queue;
  };

  class buffer_read {
    private:
    buffer buf;
    void *dst;
    std::size_t bytes;
    std::size_t offsetbytes;

    public:
    //! Setup to read from a buffer
    /*! Specify destination, length, and possibly an offset
     */
    buffer_read(const buffer &buf, void *dst, std::size_t cb,
        std::size_t os = 0);
    buffer_read(const buffer_read &br);
    buffer_read &operator=(const buffer_read &br);
    ~buffer_read(void);

    void swap(buffer_read &br);

    friend class queue;
  };

  class buffer_write {
    private:
    buffer buf;
    void *src;
    std::size_t bytes;
    std::size_t offsetbytes;

    public:
    //! Setup to write to a buffer
    /*! Specify source, length, and possibly an offset
     */
    buffer_write(const buffer &buf, void *src, std::size_t cb,
        std::size_t os = 0);
    buffer_write(const buffer_write &bw);
    buffer_write &operator=(const buffer_write &bw);
    ~buffer_write(void);

    void swap(buffer_write &bw);

    friend class queue;
  };


  class queue {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit queue(const impl &i);

    public:
    queue(const queue &q);
    queue &operator=(const queue &q);
    ~queue(void);

    //! Create command queue for a device in a context
    static queue create(const context &c, const device &d);

    //! Enqueue a data-parallel kernel
    /*! Takes an optional list of events which need to complete before this
     *  kernel should be run.
     */
    event add(const nd_run &nd,
        const std::vector<event> &waitlist = std::vector<event>());

    //! Enqueue a buffer read
    /*! Takes an optional list of events which need to complete before this
     *  read should proceed. Can be blocking or non-blocking (default).
     */
    event add(const buffer_read &br,
        const std::vector<event> &waitlist = std::vector<event>(),
        bool blocking = false);

    //! Enqueue a buffer write
    /*! Takes an optional list of events which need to complete before this
     *  write should proceed. Can be blocking or non-blocking (default).
     */
    event add(const buffer_write &bw,
        const std::vector<event> &waitlist = std::vector<event>(),
        bool blocking = true);
  };
};

namespace std {
  template<> void swap(cl::nd_run &a, cl::nd_run &b);
  template<> void swap(cl::buffer_read &a, cl::buffer_read &b);
  template<> void swap(cl::buffer_write &a, cl::buffer_write &b);
};

#endif /* CLPP_CL_QUEUE_HEADER_INCLUDED */
