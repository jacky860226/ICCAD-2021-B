#include "cell_move_router/RegionCalculator/FinalRegion.hpp"
namespace cell_move_router {
namespace RegionCalculator {

std::vector<int> FinalRegion::calSupply() {
  std::vector<int> Supply(Codec.max());
  for (int Row = InputPtr->getRowBeginIdx(); Row <= InputPtr->getRowEndIdx();
       ++Row) {
    for (int Col = InputPtr->getColBeginIdx(); Col <= InputPtr->getColEndIdx();
         ++Col) {
      unsigned long long R = Row - InputPtr->getRowBeginIdx();
      unsigned long long C = Col - InputPtr->getColBeginIdx();
      auto Coord = Codec.encode({R, C});
      Supply.at(Coord) = 0;
      for (auto &Lay : InputPtr->getLayers()) {
        Supply.at(Coord) +=
            GridManagerPtr->getGrid(Row, Col, Lay.getIdx()).getSupply();
      }
    }
  }
  return Supply;
}

std::unordered_map<const Input::Processed::CellInst *, std::pair<int, int>>
FinalRegion::calFinalPos() {
  std::unordered_map<const Input::Processed::CellInst *, std::pair<int, int>>
      FinalPos;
  std::vector<const Input::Processed::CellInst *> MovableCellList;
  for (auto &Cell : InputPtr->getCellInsts()) {
    if (Cell.isMovable() && InputPtr->getRelativeNetsMap().at(&Cell).size()) {
      FinalPos.emplace(&Cell, GridManagerPtr->getCellCoordinate(&Cell));
      MovableCellList.emplace_back(&Cell);
    }
  }
  bool Moved = false;
  do {
    Moved = false;
    for (auto CellPtr : MovableCellList) {
      int RowBeginIdx = 0, RowEndIdx = 0, ColBeginIdx = 0, ColEndIdx = 0;
      std::tie(RowBeginIdx, RowEndIdx, ColBeginIdx, ColEndIdx) =
          OptimalRegion::getRegion(CellPtr);
      int CurRow = 0, CurCol = 0;
      std::tie(CurRow, CurCol) = GridManagerPtr->getCellCoordinate(CellPtr);
      if (CurRow < RowBeginIdx || RowEndIdx < CurRow || CurCol < ColBeginIdx ||
          ColEndIdx < CurCol) {
        int NewRow = 0, NewCol = 0;
        std::tie(NewRow, NewCol) = getBestPositionOfOptimalRegion(
            CellPtr, RowBeginIdx, RowEndIdx, ColBeginIdx, ColEndIdx);
        GridManagerPtr->setCellCoordinate(CellPtr, NewRow, NewCol);
        Moved = true;
      }
    }
  } while (Moved);
  for (auto CellPtr : MovableCellList) {
    auto CurPos = GridManagerPtr->getCellCoordinate(CellPtr);
    auto OldPos = FinalPos.find(CellPtr);
    GridManagerPtr->setCellCoordinate(CellPtr, OldPos->second.first,
                                      OldPos->second.second);
    OldPos->second = CurPos;
  }
  return FinalPos;
}
std::pair<int, int> FinalRegion::getBestPositionOfOptimalRegion(
    const Input::Processed::CellInst *CellPtr, int RowBeginIdx, int RowEndIdx,
    int ColBeginIdx, int ColEndIdx) {
  int NewRow = 0, NewCol = 0;
  std::tie(NewRow, NewCol) = GridManagerPtr->getCellCoordinate(CellPtr);
  int MaxSupply = 0;
  for (auto R = RowBeginIdx; R <= RowEndIdx; ++R) {
    for (auto C = ColBeginIdx; C <= ColEndIdx; ++C) {
      auto Coord =
          Codec.encode({(R - RowBeginIdx) * 1uLL, (C - ColBeginIdx) * 1uLL});
      auto CurSupply = Supply.at(Coord);
      if (MaxSupply < CurSupply) {
        MaxSupply = CurSupply;
        NewRow = R;
        NewCol = C;
      }
    }
  }
  return {NewRow, NewCol};
}

std::tuple<int, int, int, int>
FinalRegion::getRegion(const Input::Processed::CellInst *Cell) {
  int RowBeginIdx = 0, RowEndIdx = 0;
  int ColBeginIdx = 0, ColEndIdx = 0;
  std::tie(RowBeginIdx, ColBeginIdx) = std::tie(RowEndIdx, ColEndIdx) =
      FinalPos.at(Cell);

  if (RowBeginIdx >= InputPtr->getRowBeginIdx() + 1)
    --RowBeginIdx;
  if (RowEndIdx + 1 <= InputPtr->getRowEndIdx())
    ++RowEndIdx;
  if (ColBeginIdx >= InputPtr->getColBeginIdx() + 1)
    --ColBeginIdx;
  if (ColEndIdx + 1 <= InputPtr->getColEndIdx())
    ++ColEndIdx;
  return {RowBeginIdx, RowEndIdx, ColBeginIdx, ColEndIdx};
}

} // namespace RegionCalculator
} // namespace cell_move_router