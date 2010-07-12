#include "error.hh"

cl::error::error(const std::string &what)
  : runtime_error(what)
{
}
