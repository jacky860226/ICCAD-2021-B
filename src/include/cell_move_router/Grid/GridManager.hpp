#pragma once
#include "cell_move_router/CoordinateCodec.hpp"
#include "cell_move_router/Grid/Grid.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include <vector>
namespace cell_move_router {
namespace Grid {
class GridManager {
private:
  const Input::Processed::Input *InputPtr;
  const CoordinateCodec<unsigned long long> Codec;
  std::vector<Grid> Grids;

public:
  GridManager(const Input::Processed::Input *InputPtr);
  // getGrid()
  // TODO
};
} // namespace Grid
} // namespace cell_move_router