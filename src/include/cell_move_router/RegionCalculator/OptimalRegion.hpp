#pragma once
#include "cell_move_router/Grid/GridManager.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include "cell_move_router/RegionCalculator/RegionCalculator.hpp"
namespace cell_move_router {
namespace RegionCalculator {

class OptimalRegion : public RegionCalculator {
protected:
  Grid::GridManager *GridManagerPtr;
  const Input::Processed::Input *InputPtr;

public:
  OptimalRegion(Grid::GridManager *GridManagerPtr)
      : GridManagerPtr(GridManagerPtr),
        InputPtr(GridManagerPtr->getInputPtr()) {}
  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::CellInst *Cell) override;
};

} // namespace RegionCalculator
} // namespace cell_move_router