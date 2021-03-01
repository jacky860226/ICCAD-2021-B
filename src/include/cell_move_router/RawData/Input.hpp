#pragma once
#include <ostream>

namespace cell_move_router {
namespace RawData {
class Input {
  // TODO
public:
  void to_ostream(std::ostream &out) const;
};
} // namespace RawData
} // namespace cell_move_router