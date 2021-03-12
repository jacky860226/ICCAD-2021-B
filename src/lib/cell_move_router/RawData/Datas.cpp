#include "cell_move_router/RawData/Datas.hpp"
namespace cell_move_router {
namespace RawData {

Layer::Layer(std::string &&LayerName, const int Idx, const char RoutingDir,
             const int DefaultSupplyOfOneGGrid, const double PowerFactor)
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
      << Blkgs.size();
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
  out << "CellInst" << InstName << ' ' << MasterCellName << ' ' << GGridRowIdx
      << ' ' << GGridColIdx << ' ' << (IsMovable ? "Movable" : "Fixed") << '\n';
}

// TODO

void Net::to_ostream(std::ostream &out) const {}
void VoltageArea::to_ostream(std::ostream &out) const {}
void Route::to_ostream(std::ostream &out) const {}
} // namespace RawData
} // namespace cell_move_router