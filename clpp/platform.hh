#ifndef CLPP_CL_PLATFORM_HH_INCLUDED
#define CLPP_CL_PLATFORM_HH_INCLUDED

#include <memory>
#include <string>
#include <vector>

namespace cl {
  class device;

  class platform {
    private:
    class impl;
    std::auto_ptr<impl> pimpl;

    explicit platform(const impl &i);

    public:
    //! Obtain available platforms
    static std::vector<platform> get_platforms(void);

    platform(const platform &plat);
    ~platform(void);

    platform &operator=(const platform &rhs);

    //! Get platform name
    std::string name(void) const;
    //! Get platform version
    std::string version(void) const;

    friend class device;
  };
}

#endif /* CLPP_CL_PLATFORM_HH_INCLUDED */
