#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/Input/Raw/Datas.hpp"
#include <ostream>
#include <vector>

namespace cell_move_router {
namespace Input {
namespace Raw {
class Input : Util::Outputable {
  const unsigned MaxCellMove;
  const int RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx;
  const std::vector<Layer> Layers;
  const std::vector<NonDefaultSupplyGGrid> NonDefaultSupplyGGrids;
  const std::vector<MasterCell> MasterCells;
  const std::vector<CellInst> CellInsts;
  const std::vector<Net> Nets;
  const std::vector<VoltageArea> VoltageAreas;
  const std::vector<Route> Routes;

public:
  Input(unsigned MaxCellMove, int RowBeginIdx, int ColBeginIdx, int RowEndIdx,
        int ColEndIdx, std::vector<Layer> &&Layers,
        std::vector<NonDefaultSupplyGGrid> &&NonDefaultSupplyGGrids,
        std::vector<MasterCell> &&MasterCells,
        std::vector<CellInst> &&CellInsts, std::vector<Net> &&Nets,
        std::vector<VoltageArea> &&VoltageAreas, std::vector<Route> &&Routes);
  void to_ostream(std::ostream &out) const override;
  unsigned getMaxCellMove() const { return MaxCellMove; }
  int getRowBeginIdx() const { return RowBeginIdx; }
  int getColBeginIdx() const { return ColBeginIdx; }
  int getRowEndIdx() const { return RowEndIdx; }
  int getColEndIdx() const { return ColEndIdx; }
  const std::vector<Layer> &getLayers() const { return Layers; }
  const std::vector<NonDefaultSupplyGGrid> &getNonDefaultSupplyGGrids() const {
    return NonDefaultSupplyGGrids;
  }
  const std::vector<MasterCell> &getMasterCells() const { return MasterCells; }
  const std::vector<CellInst> &getCellInsts() const { return CellInsts; }
  const std::vector<Net> &getNets() const { return Nets; }
  const std::vector<VoltageArea> &getVoltageAreas() const {
    return VoltageAreas;
  }
  const std::vector<Route> &getRoutes() const { return Routes; }
};
} // namespace Raw
} // namespace Input
} // namespace cell_move_router