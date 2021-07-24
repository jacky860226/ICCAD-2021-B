#pragma once
#include "cell_move_router/Grid/GridManager.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"

namespace cell_move_router {
class Solver {
  const Input::Processed::Input *InputPtr;
  Grid::GridManager GridManager;

public:
  Solver(const Input::Processed::Input *InputPtr)
      : InputPtr(InputPtr), GridManager(InputPtr) {}
  void solve();
  const Grid::GridManager &getGridManager() const { return GridManager; }
};
} // namespace cell_move_router