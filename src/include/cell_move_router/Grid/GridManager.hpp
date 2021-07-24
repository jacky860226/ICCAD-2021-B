#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/CoordinateCodec.hpp"
#include "cell_move_router/Grid/CellGrid.hpp"
#include "cell_move_router/Grid/Grid.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
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
                     std::pair<std::vector<Input::Processed::Route>, unsigned>>
      NetRoutes;
  std::unordered_map<const Input::Processed::CellInst *,
                     std::unordered_set<unsigned long long>>
      CellVoltageArea;

  std::vector<Input::Processed::Net *> NetPtrs;

  unsigned long long coordinateTrans(int R, int C, int L) const;
  std::tuple<int, int, int> coordinateInv(unsigned long long Coordinate) const;

public:
  GridManager(const Input::Processed::Input *InputPtr);
  const Input::Processed::Input *getInputPtr() const { return InputPtr; }
  void addNet(const Input::Processed::Net *Net,
              std::vector<Input::Processed::Route> &&Routes);
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
                     std::pair<std::vector<Input::Processed::Route>, unsigned>>
      &getNetRoutes() {
    return NetRoutes;
  }
  void to_ostream(std::ostream &out) const override {
    std::vector<const std::pair<const Input::Processed::CellInst *const,
                                unsigned long long> *>
        MovedCellInsts;
    for (auto &P : CellCoordinate) {
      if (coordinateTrans(P.first->getGGridRowIdx(), P.first->getGGridColIdx(),
                          1) != P.second) {
        MovedCellInsts.emplace_back(&P);
      }
    }
    out << "NumMovedCellInst " << MovedCellInsts.size() << '\n';
    for (auto P : MovedCellInsts) {
      auto Coord = coordinateInv(P->second);
      out << "CellInst " << P->first->getInstName() << ' ' << std::get<0>(Coord)
          << ' ' << std::get<1>(Coord) << '\n';
    }
    int NumRoute = 0;
    for (const auto &NetRoute : NetRoutes) {
      NumRoute += NetRoute.second.first.size();
    }
    out << "NumRoutes " << NumRoute << '\n';
    for (const auto &NetRoute : NetRoutes) {
      for (const auto Route : NetRoute.second.first) {
        Route.to_ostream(out);
      }
    }
  }
};
} // namespace Grid
} // namespace cell_move_router