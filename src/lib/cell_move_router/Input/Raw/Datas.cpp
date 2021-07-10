#include "cell_move_router/Input/Raw/Datas.hpp"
namespace cell_move_router {
namespace Input {
namespace Raw {

Layer::Layer(std::string &&LayerName, const int Idx, const char RoutingDir,
             const int DefaultSupplyOfOneGGrid, const long long PowerFactor)
    : LayerName(LayerName), Idx(Idx), RoutingDir(RoutingDir),
      DefaultSupplyOfOneGGrid(DefaultSupplyOfOneGGrid),
      PowerFactor(PowerFactor) {}
void Layer::to_ostream(std::ostream &out) const {
  out << "Lay " << LayerName << ' ' << Idx << ' ' << RoutingDir << ' '
      << DefaultSupplyOfOneGGrid << ' ' << PowerFactor << '\n';
}

NonDefaultSupplyGGrid::NonDefaultSupplyGGrid(const int RowIdx, const int ColIdx,
                                             const int LayIdx,
                                             const int IncrOrDecrValue)
    : RowIdx(RowIdx), ColIdx(ColIdx), LayIdx(LayIdx),
      IncrOrDecrValue(IncrOrDecrValue) {}

void NonDefaultSupplyGGrid::to_ostream(std::ostream &out) const {
  auto OutputWithSign = [&](int x) {
    std::string Ans;
    if (x >= 0)
      Ans = "+";
    Ans += std::to_string(x);
    return Ans;
  };
  out << RowIdx << ' ' << ColIdx << ' ' << LayIdx << ' '
      << OutputWithSign(IncrOrDecrValue) << '\n';
}

MasterCell::MasterCell(std::string &&MasterCellName, std::vector<Pin> &&Pins,
                       std::vector<Blkg> &&Blkgs)
    : MasterCellName(MasterCellName), Pins(Pins), Blkgs(Blkgs) {}

void MasterCell::to_ostream(std::ostream &out) const {
  out << "MasterCell " << MasterCellName << ' ' << Pins.size() << ' '
      << Blkgs.size() << '\n';
  for (const auto &pin : Pins) {
    pin.to_ostream(out);
  }
  for (const auto &blkg : Blkgs) {
    blkg.to_ostream(out);
  }
}

MasterCell::Pin::Pin(std::string &&PinName, std::string &&PinLayer)
    : PinName(PinName), PinLayer(PinLayer) {}

void MasterCell::Pin::to_ostream(std::ostream &out) const {
  out << "Pin " << PinName << ' ' << PinLayer << '\n';
}

MasterCell::Blkg::Blkg(std::string &&BlockageName, std::string &&BlockageLayer,
                       const int Demand)
    : BlockageName(BlockageName), BlockageLayer(BlockageLayer), Demand(Demand) {
}

void MasterCell::Blkg::to_ostream(std::ostream &out) const {
  out << "Blkg " << BlockageName << ' ' << BlockageLayer << ' ' << Demand
      << '\n';
}

CellInst::CellInst(std::string &&InstName, std::string &&MasterCellName,
                   const int GGridRowIdx, const int GGridColIdx,
                   const bool IsMovable)
    : InstName(InstName), MasterCellName(MasterCellName),
      GGridRowIdx(GGridRowIdx), GGridColIdx(GGridColIdx), IsMovable(IsMovable) {
}

void CellInst::to_ostream(std::ostream &out) const {
  out << "CellInst " << InstName << ' ' << MasterCellName << ' ' << GGridRowIdx
      << ' ' << GGridColIdx << ' ' << (IsMovable ? "Movable" : "Fixed") << '\n';
}

Net::Net(std::string &&NetName, std::vector<Net::Pin> &&Pins,
         std::string &&MinRoutingLayConstraint, const long long Weight)
    : NetName(NetName), Pins(Pins),
      MinRoutingLayConstraint(MinRoutingLayConstraint), Weight(Weight) {}

void Net::to_ostream(std::ostream &out) const {
  out << "Net " << NetName << ' ' << Pins.size() << ' '
      << MinRoutingLayConstraint << ' ' << Weight << '\n';
  for (auto pin : Pins) {
    pin.to_ostream(out);
  }
}

Net::Pin::Pin(std::string &&InstName, std::string &&MasterPinName)
    : InstName(InstName), MasterPinName(MasterPinName) {}

void Net::Pin::to_ostream(std::ostream &out) const {
  out << "Pin " << InstName << '/' << MasterPinName << '\n';
}

VoltageArea::GGrid::GGrid(const int GGridRowIdx, const int GGridColIdx)
    : GGridRowIdx(GGridRowIdx), GGridColIdx(GGridColIdx) {}

void VoltageArea::GGrid::to_ostream(std::ostream &out) const {
  out << GGridRowIdx << ' ' << GGridColIdx << '\n';
}

VoltageArea::Instance::Instance(std::string &&InstanceName)
    : InstanceName(InstanceName) {}

void VoltageArea::Instance::to_ostream(std::ostream &out) const {
  out << InstanceName << '\n';
}

VoltageArea::VoltageArea(std::string &&VoltageAreaName,
                         std::vector<VoltageArea::GGrid> &&GGrids,
                         std::vector<VoltageArea::Instance> &&Instances)
    : VoltageAreaName(VoltageAreaName), GGrids(GGrids), Instances(Instances) {}

void VoltageArea::to_ostream(std::ostream &out) const {
  out << "Name " << VoltageAreaName << '\n';
  out << "GGrids " << GGrids.size() << '\n';
  for (auto grid : GGrids) {
    grid.to_ostream(out);
  }
  out << "Instances " << Instances.size() << '\n';
  for (auto instance : Instances) {
    instance.to_ostream(out);
  }
}

Route::Route(const int SRowIdx, const int SColIdx, const int SLayIdx,
             const int ERowIdx, const int EColIdx, const int ELayIdx,
             std::string &&NetName)
    : SRowIdx(SRowIdx), SColIdx(SColIdx), SLayIdx(SLayIdx), ERowIdx(ERowIdx),
      EColIdx(EColIdx), ELayIdx(ELayIdx), NetName(NetName) {}

void Route::to_ostream(std::ostream &out) const {
  out << SRowIdx << ' ' << SColIdx << ' ' << SLayIdx << ' ' << ERowIdx << ' '
      << EColIdx << ' ' << ELayIdx << ' ' << NetName << '\n';
}
} // namespace Raw
} // namespace Input
} // namespace cell_move_router