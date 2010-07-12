#ifndef CLPP_CL_EVENT_HH_INCLUDED
#define CLPP_CL_EVENT_HH_INCLUDED

#include <memory>
#include <vector>

namespace cl {
  class queue;

  class event {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit event(const impl &i);
    public:
    event(const event &e);
    event &operator=(const event &e);
    ~event(void);

    //! Wait for this event to complete
    void wait(void) const;

    //! Wait for all events to complete
    static void wait_all(const std::vector<event> &evs);

    //! Check to see if this event has completed
    bool completed(void) const;

    friend class queue;
  };
};

#endif /* CLPP_CL_EVENT_HH_INCLUDED */
