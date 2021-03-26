#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/Input/Raw/Datas.hpp"
#include <ostream>
#include <vector>

namespace cell_move_router {
namespace Input {
namespace Processed {
class Input : Util::Outputable {
  // TODO

public:
  void to_ostream(std::ostream &out) const override;
};
} // namespace Processed
} // namespace Input
} // namespace cell_move_router