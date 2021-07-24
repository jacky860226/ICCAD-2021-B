#include "cell_move_router/Input/Processed/Input.hpp"
#include <algorithm>

namespace cell_move_router {
namespace Input {
namespace Processed {

std::unordered_map<std::string, const Raw::Layer *> Input::CreateLayerMap() {
  std::unordered_map<std::string, const Raw::Layer *> LayerMap;
  LayerMap["NoCstr"] = &RawInput->getLayers().at(0);
  for (const auto &Layer : RawInput->getLayers())
    LayerMap[Layer.getLayerName()] = &Layer;
  return LayerMap;
}

std::unordered_map<std::string, const MasterCell *>
Input::CreateMasterCellMap() {
  std::unordered_map<std::string, const MasterCell *> MasterCellMap;
  for (const auto &MasterCell : MasterCells)
    MasterCellMap[MasterCell.getMasterCellName()] = &MasterCell;
  return MasterCellMap;
}

std::unordered_map<std::string, const CellInst *> Input::CreateCellInstMap() {
  std::unordered_map<std::string, const CellInst *> CellInstMap;
  for (const auto &CellInst : CellInsts)
    CellInstMap[CellInst.getInstName()] = &CellInst;
  return CellInstMap;
}

std::unordered_map<std::string, const Net *> Input::CreateNetMap() {
  std::unordered_map<std::string, const Net *> NetMap;
  for (const auto &Net : Nets)
    NetMap[Net.getNetName()] = &Net;
  return NetMap;
}

std::vector<MasterCell> Input::CreateMasterCells() {
  std::vector<MasterCell> MasterCells;
  for (const auto &M : RawInput->getMasterCells())
    MasterCells.emplace_back(&M, LayerMap);
  return MasterCells;
}

std::vector<CellInst> Input::CreateCellInsts() {
  std::vector<CellInst> CellInsts;
  for (const auto &C : RawInput->getCellInsts())
    CellInsts.emplace_back(&C, MasterCellMap);
  return CellInsts;
}

std::vector<Net> Input::CreateNets() {
  std::vector<Net> Nets;
  for (const auto &N : RawInput->getNets())
    Nets.emplace_back(&N, CellInstMap, LayerMap);
  return Nets;
}

std::vector<VoltageArea> Input::CreateVoltageAreas() {
  std::vector<VoltageArea> VoltageAreas;
  for (const auto &V : RawInput->getVoltageAreas())
    VoltageAreas.emplace_back(&V, CellInstMap);
  return VoltageAreas;
}

std::vector<Route> Input::CreateRoutes() {
  std::vector<Route> Routes;
  for (const auto &R : RawInput->getRoutes())
    Routes.emplace_back(&R, NetMap);
  return Routes;
}

std::unordered_map<const CellInst *, std::vector<const Net *>>
Input::CreateRelativeNetsMap() {
  std::unordered_map<const CellInst *, std::vector<const Net *>>
      RelativeNetsMap;
  for (const auto &C : getCellInsts()) {
    RelativeNetsMap.emplace(&C, std::vector<const Net *>());
  }
  for (const auto &N : getNets()) {
    for (const auto &Pin : N.getPins())
      RelativeNetsMap[Pin.getInst()].emplace_back(&N);
  }
  for (auto &P : RelativeNetsMap) {
    std::sort(P.second.begin(), P.second.end());
    P.second.erase(std::unique(P.second.begin(), P.second.end()),
                   P.second.end());
  }
  return RelativeNetsMap;
}

Input::Input(std::unique_ptr<Raw::Input> &&RawInput)
    : RawInput(std::move(RawInput)), LayerMap(CreateLayerMap()),
      MasterCells(CreateMasterCells()), MasterCellMap(CreateMasterCellMap()),
      CellInsts(CreateCellInsts()), CellInstMap(CreateCellInstMap()),
      Nets(CreateNets()), NetMap(CreateNetMap()),
      VoltageAreas(CreateVoltageAreas()), Routes(CreateRoutes()),
      RelativeNetsMap(CreateRelativeNetsMap()) {}

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

} // namespace Processed
} // namespace Input
} // namespace cell_move_router