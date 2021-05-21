#include "cell_move_router/Input/Processed/Datas.hpp"

namespace cell_move_router {
namespace Input {
namespace Processed {

MasterCell::MasterCell(
    const Raw::MasterCell *RawMasterCell,
    const std::unordered_map<std::string, const Raw::Layer *> &LayerMap)
    : RawMasterCell(RawMasterCell),
      PinMap(MasterCell::CreatePinMap(RawMasterCell, LayerMap)),
      BlkgMap(MasterCell::CreateBlkgMap(RawMasterCell, LayerMap)) {}

void MasterCell::to_ostream(std::ostream &out) const {
  RawMasterCell->to_ostream(out);
}

MasterCell::Pin::Pin(const Raw::Layer *PinLayer) : PinLayer(PinLayer) {}

MasterCell::Blkg::Blkg(const Raw::Layer *BlockageLayer, const int Demand)
    : BlockageLayer(BlockageLayer), Demand(Demand) {}

std::unordered_map<std::string, MasterCell::Pin> MasterCell::CreatePinMap(
    const Raw::MasterCell *RawMasterCell,
    const std::unordered_map<std::string, const Raw::Layer *> &LayerMap) {
  std::unordered_map<std::string, MasterCell::Pin> PinMap;
  for (const auto &Pin : RawMasterCell->getPins())
    PinMap.emplace(Pin.getPinName(),
                   MasterCell::Pin(LayerMap.at(Pin.getPinLayer())));
  return PinMap;
}
std::unordered_map<std::string, MasterCell::Blkg> MasterCell::CreateBlkgMap(
    const Raw::MasterCell *RawMasterCell,
    const std::unordered_map<std::string, const Raw::Layer *> &LayerMap) {
  std::unordered_map<std::string, MasterCell::Blkg> BlkgMap;
  for (const auto &Blkg : RawMasterCell->getBlkgs()) {
    BlkgMap.emplace(Blkg.getBlockageName(),
                    MasterCell::Blkg(LayerMap.at(Blkg.getBlockageLayer()),
                                     Blkg.getDemand()));
  }
  return BlkgMap;
}

void CellInst::to_ostream(std::ostream &out) const {
  RawCellInst->to_ostream(out);
}

CellInst::CellInst(
    const Raw::CellInst *RawCellInst,
    const std::unordered_map<std::string, const MasterCell *> &MasterCellMap)
    : RawCellInst(RawCellInst),
      ProcessedMasterCell(
          MasterCellMap.at(RawCellInst->Raw::CellInst::getMasterCellName())) {}

Net::Pin::Pin(const CellInst *Inst, const MasterCell::Pin *MasterPin)
    : Inst(Inst), MasterPin(MasterPin) {}

std::vector<Net::Pin> Net::CreatePins(
    const Raw::Net *RawNet,
    const std::unordered_map<std::string, const CellInst *> &CellInstMap) {
  std::vector<Net::Pin> Pins;
  for (const auto &P : RawNet->getPins()) {
    auto Inst = CellInstMap.at(P.getInstName());
    Pins.emplace_back(
        Inst, &Inst->getMasterCell()->getPinMap().at(P.getMasterPinName()));
  }
  return Pins;
}

Net::Net(const Raw::Net *RawNet,
         const std::unordered_map<std::string, const CellInst *> &CellInstMap,
         const std::unordered_map<std::string, const Raw::Layer *> &LayerMap)
    : RawNet(RawNet), Pins(CreatePins(RawNet, CellInstMap)),
      MinRoutingLayConstraint(
          LayerMap.at(RawNet->getMinRoutingLayConstraint())) {}

void Net::to_ostream(std::ostream &out) const { RawNet->to_ostream(out); }

std::vector<const CellInst *> VoltageArea::CreateInstances(
    const Raw::VoltageArea *RawVoltageArea,
    const std::unordered_map<std::string, const CellInst *> &CellInstMap) {
  std::vector<const CellInst *> Instances;
  for (const auto &Inst : RawVoltageArea->getInstances())
    Instances.emplace_back(CellInstMap.at(Inst.getInstanceName()));
  return Instances;
}

VoltageArea::VoltageArea(
    const Raw::VoltageArea *RawVoltageArea,
    const std::unordered_map<std::string, const CellInst *> &CellInstMap)
    : RawVoltageArea(RawVoltageArea),
      Instances(CreateInstances(RawVoltageArea, CellInstMap)) {}

void VoltageArea::to_ostream(std::ostream &out) const {
  RawVoltageArea->to_ostream(out);
}

Route::Route(const int SRowIdx, const int SColIdx, const int SLayIdx,
             const int ERowIdx, const int EColIdx, const int ELayIdx,
             const Net *NetPtr)
    : SRowIdx(std::min(SRowIdx, ERowIdx)), SColIdx(std::min(SColIdx, ERowIdx)),
      SLayIdx(std::min(SLayIdx, ELayIdx)), ERowIdx(std::max(SRowIdx, ERowIdx)),
      EColIdx(std::max(SColIdx, EColIdx)), ELayIdx(std::max(SLayIdx, ELayIdx)),
      NetPtr(NetPtr) {}

Route::Route(const Raw::Route *RawRoute,
             const std::unordered_map<std::string, const Net *> &NetMap)
    : Route(RawRoute->getSRowIdx(), RawRoute->getSColIdx(),
            RawRoute->getSLayIdx(), RawRoute->getERowIdx(),
            RawRoute->getEColIdx(), RawRoute->getELayIdx(),
            NetMap.at(RawRoute->getNetName())) {}

void Route::to_ostream(std::ostream &out) const {
  out << SRowIdx << ' ' << SColIdx << ' ' << SLayIdx << ' ' << ERowIdx << ' '
      << EColIdx << ' ' << ELayIdx << ' ' << NetPtr->getNetName() << '\n';
}
} // namespace Processed
} // namespace Input
} // namespace cell_move_router