#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/CoordinateCodec.hpp"
#include "cell_move_router/Grid/CellGrid.hpp"
#include "cell_move_router/Grid/Grid.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include <limits>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cell_move_router {
namespace Grid {
class GridManager : Util::Outputable {
  const Input::Processed::Input *InputPtr;
  const CoordinateCodec<unsigned long long> Codec;
  std::vector<Grid> Grids;
  std::vector<CellGrid> CellGrids;
  std::unordered_map<const Input::Processed::CellInst *, unsigned long long>
      CellCoordinate;
  std::unordered_map<const Input::Processed::Net *,
                     std::pair<std::vector<Input::Processed::Route>, long long>>
      NetRoutes;
  std::unordered_map<const Input::Processed::CellInst *,
                     std::unordered_set<unsigned long long>>
      CellVoltageArea;

  std::vector<Input::Processed::Net *> NetPtrs;

  std::unordered_set<unsigned> OverflowGrids;

  unsigned Tag;
  long long CurrentCost;

  unsigned long long coordinateTrans(int R, int C, int L) const;
  std::tuple<int, int, int> coordinateInv(unsigned long long Coordinate) const;

  void increaseTag() {
    if (Tag == std::numeric_limits<unsigned>::max()) {
      Tag = 1;
      for (auto &G : Grids)
        G.getTag() = 0;
    } else {
      ++Tag;
    }
  }

public:
  GridManager(const Input::Processed::Input *InputPtr);
  const Input::Processed::Input *getInputPtr() const { return InputPtr; }
  void addNet(const Input::Processed::Net *Net);
  void removeNet(const Input::Processed::Net *Net);
  void addCell(const Input::Processed::CellInst *CellInst, const int Row,
               const int Col);
  void removeCell(const Input::Processed::CellInst *CellInst);
  const Grid &getGrid(int R, int C, int L) const {
    return Grids.at(coordinateTrans(R, C, L));
  }
  const CellGrid &getCellGrid(int R, int C) const {
    return CellGrids.at(coordinateTrans(R, C, 1));
  }
  std::pair<int, int>
  getCellCoordinate(const Input::Processed::CellInst *CellInst) const {
    auto Coord = coordinateInv(CellCoordinate.at(CellInst));
    return {std::get<0>(Coord), std::get<1>(Coord)};
  }
  void setCellCoordinate(const Input::Processed::CellInst *CellInst,
                         const int Row, const int Col) {
    CellCoordinate.emplace(CellInst, coordinateTrans(Row, Col, 1));
  }
  std::unordered_map<const Input::Processed::Net *,
                     std::pair<std::vector<Input::Processed::Route>, long long>>
      &getNetRoutes() {
    return NetRoutes;
  }
  std::pair<std::vector<Input::Processed::Route>, long long> &
  getNetRoute(const Input::Processed::Net *NetPtr) {
    return NetRoutes[NetPtr];
  }
  long long getRouteCost(const Input::Processed::Net *Net,
                         const std::vector<Input::Processed::Route> &Routes);
  long long getCurrentCost() const { return CurrentCost; }
  const std::unordered_set<unsigned long long> &
  getCellVoltageArea(const Input::Processed::CellInst *CellPtr) const {
    return CellVoltageArea.at(CellPtr);
  }
  bool isOverflow() const { return OverflowGrids.size(); }
  void to_ostream(std::ostream &out) const override;
};
} // namespace Grid
} // namespace cell_move_router