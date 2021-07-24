#pragma once
#include <memory>
namespace Util {
template <typename TO, typename FROM>
std::unique_ptr<TO> dynamic_unique_pointer_cast(std::unique_ptr<FROM> &&old) {
  // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
  if (auto P = dynamic_cast<TO *>(old.get())) {
    old.release();
    return std::unique_ptr<TO>{P};
  }
  return nullptr;
}
} // namespace Util