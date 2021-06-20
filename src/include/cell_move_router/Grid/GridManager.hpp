#pragma once
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
class GridManager {
private:
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
  std::unordered_map<const Input::Processed::Net *,
                     std::pair<std::vector<Input::Processed::Route>, unsigned>>
      &getNetRoutes() {
    return NetRoutes;
  }
  void output(std::ostream &out) { // For alpha test
    out << "NumMovedCellInst 0\n";
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