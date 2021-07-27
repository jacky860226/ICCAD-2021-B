#include "cell_move_router/Mover/Mover.hpp"
#include "cell_move_router/Router/GraphApproxRouter.hpp"
#include <unordered_map>

namespace cell_move_router {
namespace Mover {

void Mover::initalFreqMovedCell() {
  for (auto &Cell : InputPtr->getCellInsts()) {
    if (Cell.isMovable()) {
      if (GridManager.getCellVoltageArea(&Cell).size())
        continue; // TODO: handle Cell in VoltageArea
      FreqMovedCell.emplace(&Cell, 0);
    }
  }
}
bool Mover::add_and_route(const Input::Processed::CellInst *CellPtr,
                          const int Row, const int Col) {
  GridManager.addCell(CellPtr, Row, Col);
  if (GridManager.isOverflow()) {
    GridManager.removeCell(CellPtr);
    return false;
  }
  Router::GraphApproxRouter GraphApproxRouter(&GridManager);
  std::vector<std::pair<
      const Input::Processed::Net *,
      std::pair<std::vector<cell_move_router::Input::Processed::Route>,
                long long>>>
      OriginRoutes;
  bool Accept = true;
  for (auto NetPtr : InputPtr->getRelativeNetsMap().at(CellPtr)) {
    auto &OriginRoute = GridManager.getNetRoutes()[NetPtr];
    auto Pair = GraphApproxRouter.singleNetRoute(NetPtr, OriginRoute.first);
    OriginRoutes.emplace_back(NetPtr, std::move(OriginRoute));
    if (Pair.second == false) {
      Accept = false;
      break;
    }
    auto Cost = GridManager.getRouteCost(NetPtr, Pair.first);
    Input::Processed::Route::reduceRouteSegments(Pair.first);
    OriginRoute = {std::move(Pair.first), Cost};
    bool Overflow = GridManager.isOverflow();
    GridManager.addNet(NetPtr);
    assert(!GridManager.isOverflow());
  }
  if (Accept) {
    return true;
  }
  GridManager.getNetRoutes()[OriginRoutes.back().first] =
      std::move(OriginRoutes.back().second);
  OriginRoutes.pop_back();
  while (OriginRoutes.size()) {
    GridManager.removeNet(OriginRoutes.back().first);
    GridManager.getNetRoutes()[OriginRoutes.back().first] =
        std::move(OriginRoutes.back().second);
    OriginRoutes.pop_back();
  }
  GridManager.removeCell(CellPtr);
  return false;
}
void Mover::move(RegionCalculator::RegionCalculator &RC, int Round) {
  std::vector<std::pair<long long, const Input::Processed::CellInst *>>
      CellNetLength;
  for (auto &P : FreqMovedCell) {
    auto CellPtr = P.first;
    long long NetLength = 0;
    for (auto NetPtr : InputPtr->getRelativeNetsMap().at(CellPtr)) {
      NetLength += GridManager.getNetRoutes()[NetPtr].second;
    }
    CellNetLength.emplace_back(NetLength, CellPtr);
  }
  std::sort(
      CellNetLength.begin(), CellNetLength.end(),
      std::greater<std::pair<long long, const Input::Processed::CellInst *>>());
  unsigned MoveCnt = 0;
  for (auto &P : CellNetLength) {
    auto CellPtr = P.second;
    int RowBeginIdx = 0, RowEndIdx = 0, ColBeginIdx = 0, ColEndIdx = 0;
    std::tie(RowBeginIdx, RowEndIdx, ColBeginIdx, ColEndIdx) =
        RC.getRegion(CellPtr);
    std::vector<std::pair<int, int>> CandidatePos;
    for (int R = RowBeginIdx; R <= RowEndIdx; ++R) {
      for (int C = ColBeginIdx; C <= ColEndIdx; ++C) {
        CandidatePos.emplace_back(R, C);
      }
    }
    std::shuffle(CandidatePos.begin(), CandidatePos.end(), Random);
    auto OldCoord = GridManager.getCellCoordinate(CellPtr);
    {
      for (auto NetPtr : InputPtr->getRelativeNetsMap().at(CellPtr)) {
        GridManager.removeNet(NetPtr);
      }
      GridManager.removeCell(CellPtr);
    }
    bool Success = false;
    for (auto P : CandidatePos) {
      if (add_and_route(CellPtr, P.first, P.second)) {
        Success = true;
        break;
      }
    }
    if (Success)
      ++MoveCnt;
    else {
      GridManager.addCell(CellPtr, OldCoord.first, OldCoord.second);
      for (auto NetPtr : InputPtr->getRelativeNetsMap().at(CellPtr)) {
        GridManager.addNet(NetPtr);
      }
    }
    if (MoveCnt == InputPtr->getMaxCellMove())
      break;
  }
}

} // namespace Mover
} // namespace cell_move_router