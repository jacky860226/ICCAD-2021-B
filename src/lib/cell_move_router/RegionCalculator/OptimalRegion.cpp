#include "cell_move_router/RegionCalculator/OptimalRegion.hpp"
#include <algorithm>
namespace cell_move_router {
namespace RegionCalculator {

std::tuple<int, int, int, int>
OptimalRegion::getRegion(const Input::Processed::CellInst *Cell) {
  bool IsChange = false;
  std::vector<int> OptimalRows, OptimalCols;
  for (auto Net : InputPtr->getRelativeNetsMap().at(Cell)) {
    int RowBegin = std::numeric_limits<int>::max();
    int ColBegin = std::numeric_limits<int>::max();
    int RowEnd = 0, ColEnd = 0;
    for (auto &Pin : Net->getPins()) {
      auto PinCell = Pin.getInst();
      if (PinCell != Cell) {
        int PinRow = 0, PinCol = 0;
        std::tie(PinRow, PinCol) = GridManagerPtr->getCellCoordinate(PinCell);
        RowBegin = std::min(RowBegin, PinRow);
        ColBegin = std::min(ColBegin, PinCol);
        RowEnd = std::max(RowEnd, PinRow);
        ColEnd = std::max(ColEnd, PinCol);
        IsChange = true;
      }
    }

    OptimalRows.emplace_back(RowBegin);
    OptimalCols.emplace_back(ColBegin);
    OptimalRows.emplace_back(RowEnd);
    OptimalCols.emplace_back(ColEnd);
  }

  int RowBeginIdx = 0;
  int RowEndIdx = 0;
  int ColBeginIdx = 0;
  int ColEndIdx = 0;

  if (IsChange) {
    std::sort(OptimalRows.begin(), OptimalRows.end());
    std::sort(OptimalCols.begin(), OptimalCols.end());
    RowBeginIdx = OptimalRows[OptimalRows.size() / 2 - 1];
    RowEndIdx = OptimalRows[OptimalRows.size() / 2];
    ColBeginIdx = OptimalCols[OptimalCols.size() / 2 - 1];
    ColEndIdx = OptimalCols[OptimalCols.size() / 2];
  } else {
    RowBeginIdx = InputPtr->getRowBeginIdx();
    RowEndIdx = InputPtr->getRowEndIdx();
    ColBeginIdx = InputPtr->getColBeginIdx();
    ColEndIdx = InputPtr->getColEndIdx();
  }
  return {RowBeginIdx, RowEndIdx, ColBeginIdx, ColEndIdx};
}

} // namespace RegionCalculator
} // namespace cell_move_router