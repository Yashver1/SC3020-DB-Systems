#ifndef ERRORS_H
#define ERRORS_H
#include <stdexcept>

class NotImplemented : public std::logic_error {
 public:
  NotImplemented() : std::logic_error("method not implemented") {};
};

#endif  // !ERRORS_H
