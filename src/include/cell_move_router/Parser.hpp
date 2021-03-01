#pragma once
#include <cell_move_router/Input.hpp>
#include <istream>
#include <memory>

namespace cell_move_router {
class Parser {
  // TODO
public:
  std::unique_ptr<Input> parse(std::istream &input);
};
} // namespace cell_move_router