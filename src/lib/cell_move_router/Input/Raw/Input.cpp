#include "cell_move_router/Input/Raw/Input.hpp"

namespace cell_move_router {
namespace Input {
namespace Raw {
Input::Input(unsigned MaxCellMove, int RowBeginIdx, int ColBeginIdx,
             int RowEndIdx, int ColEndIdx, std::vector<Layer> &&Layers,
             std::vector<NonDefaultSupplyGGrid> &&NonDefaultSupplyGGrids,
             std::vector<MasterCell> &&MasterCells,
             std::vector<CellInst> &&CellInsts, std::vector<Net> &&Nets,
             std::vector<VoltageArea> &&VoltageAreas,
             std::vector<Route> &&Routes)
    : MaxCellMove(MaxCellMove), RowBeginIdx(RowBeginIdx),
      ColBeginIdx(ColBeginIdx), RowEndIdx(RowEndIdx), ColEndIdx(ColEndIdx),
      Layers(std::move(Layers)),
      NonDefaultSupplyGGrids(std::move(NonDefaultSupplyGGrids)),
      MasterCells(std::move(MasterCells)), CellInsts(std::move(CellInsts)),
      Nets(std::move(Nets)), VoltageAreas(std::move(VoltageAreas)),
      Routes(std::move(Routes)) {}

void Input::to_ostream(std::ostream &out) const {
  out << "MaxCellMove " << getMaxCellMove() << '\n';

  out << "GGridBoundaryIdx " << getRowBeginIdx() << ' ' << getColBeginIdx()
      << ' ' << getRowEndIdx() << ' ' << getColEndIdx() << '\n';

  out << "NumLayer " << getLayers().size() << '\n';
  for (const auto &Layer : getLayers())
    Layer.to_ostream(out);

  out << "NumNonDefaultSupplyGGrid " << getNonDefaultSupplyGGrids().size()
      << '\n';
  for (const auto &NonDefaultSupplyGGrid : getNonDefaultSupplyGGrids())
    NonDefaultSupplyGGrid.to_ostream(out);

  out << "NumMasterCell " << getMasterCells().size() << '\n';
  for (const auto &MasterCell : getMasterCells())
    MasterCell.to_ostream(out);

  out << "NumCellInst " << getCellInsts().size() << '\n';
  for (const auto &CellInst : getCellInsts())
    CellInst.to_ostream(out);

  out << "NumNets " << getNets().size() << '\n';
  for (const auto &Net : getNets())
    Net.to_ostream(out);

  out << "NumVoltageAreas " << getVoltageAreas().size() << '\n';
  for (const auto &VoltageArea : getVoltageAreas())
    VoltageArea.to_ostream(out);

  out << "NumRoutes " << getRoutes().size() << '\n';
  for (const auto &Route : getRoutes())
    Route.to_ostream(out);
}
} // namespace Raw
} // namespace Input
} // namespace cell_move_router