#pragma once
#include "cell_move_router/RawData/Input.hpp"
#include <istream>
#include <memory>

namespace cell_move_router {
class Parser {
  // TODO
public:
  std::unique_ptr<RawData::Input> parse(std::istream &input);
};
} // namespace cell_move_router