#pragma once
#include <ostream>

namespace cell_move_router {
class Input {

public:
  void to_ostream(std::ostream &out) const;
};
} // namespace cell_move_router