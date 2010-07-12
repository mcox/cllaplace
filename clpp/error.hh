#ifndef CLPP_CL_ERROR_HH_INCLUDED
#define CLPP_CL_ERROR_HH_INCLUDED

#include <string>
#include <stdexcept>

namespace cl {
  class error : public std::runtime_error {
    public:
    explicit error(const std::string &what);
  };
};

#endif /* CLPP_CL_ERROR_HH_INCLUDED */
