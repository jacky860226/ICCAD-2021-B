#pragma once
#include <ostream>
namespace Util {
class Outputable {
public:
  virtual void to_ostream(std::ostream &out) const = 0;
};
} // namespace Util