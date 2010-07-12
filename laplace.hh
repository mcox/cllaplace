#ifndef LAPLACE_HH_INCLUDED
#define LAPLACE_HH_INCLUDED

#include <stdexcept>

/* Exception indicating help activated */
class help_activated : public std::runtime_error {
  public:
  help_activated(void);
};

#endif /* LAPLACE_HH_INCLUDED */
