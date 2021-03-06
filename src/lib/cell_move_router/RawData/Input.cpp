#include "cell_move_router/RawData/Input.hpp"

namespace cell_move_router {
namespace RawData {
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
  out << "MaxCellMove " << MaxCellMove << '\n';

  out << "GGridBoundaryIdx " << RowBeginIdx << ' ' << ColBeginIdx << ' '
      << RowEndIdx << ' ' << ColEndIdx << '\n';

  out << "NumLayer " << Layers.size() << '\n';
  for (const auto &Layer : Layers)
    Layer.to_ostream(out);

  out << "NumNonDefaultSupplyGGrid " << NonDefaultSupplyGGrids.size() << '\n';
  for (const auto &NonDefaultSupplyGGrid : NonDefaultSupplyGGrids)
    NonDefaultSupplyGGrid.to_ostream(out);

  out << "NumMasterCell " << MasterCells.size() << '\n';
  for (const auto &MasterCell : MasterCells)
    MasterCell.to_ostream(out);

  out << "NumCellInst " << CellInsts.size() << '\n';
  for (const auto &CellInst : CellInsts)
    CellInst.to_ostream(out);

  out << "NumNets " << Nets.size() << '\n';
  for (const auto &Net : Nets)
    Net.to_ostream(out);

  out << "NumVoltageAreas " << VoltageAreas.size() << '\n';
  for (const auto &VoltageArea : VoltageAreas)
    VoltageArea.to_ostream(out);

  out << "NumRoutes " << Routes.size() << '\n';
  for (const auto &Route : Routes)
    Route.to_ostream(out);
}
} // namespace RawData
} // namespace cell_move_router