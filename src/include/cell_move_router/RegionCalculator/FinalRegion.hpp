#pragma once
#include "cell_move_router/Grid/GridManager.hpp"
#include "cell_move_router/RegionCalculator/OptimalRegion.hpp"
#include <tuple>
#include <unordered_map>
#include <vector>

namespace cell_move_router {
namespace RegionCalculator {

class FinalRegion : public OptimalRegion {
  const CoordinateCodec<unsigned long long> Codec;
  const std::vector<int> Supply;
  const std::unordered_map<const Input::Processed::CellInst *,
                           std::pair<int, int>>
      FinalPos;

  std::vector<int> calSupply();
  std::unordered_map<const Input::Processed::CellInst *, std::pair<int, int>>
  calFinalPos();
  std::pair<int, int>
  getBestPositionOfOptimalRegion(const Input::Processed::CellInst *CellPtr,
                                 int RowBeginIdx, int RowEndIdx,
                                 int ColBeginIdx, int ColEndIdx);

public:
  FinalRegion(Grid::GridManager *GridManagerPtr)
      : OptimalRegion(GridManagerPtr),
        Codec({InputPtr->getRowSize(), InputPtr->getColsize()}),
        Supply(calSupply()), FinalPos(calFinalPos()) {}

  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::CellInst *Cell) override;
};

} // namespace RegionCalculator
} // namespace cell_move_router