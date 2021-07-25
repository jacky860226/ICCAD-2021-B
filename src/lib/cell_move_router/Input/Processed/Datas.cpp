#include "cell_move_router/Input/Processed/Datas.hpp"
#include <algorithm>
#include <cassert>

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

Route::Route(const int _SRowIdx, const int _SColIdx, const int _SLayIdx,
             const int _ERowIdx, const int _EColIdx, const int _ELayIdx,
             const Net *NetPtr)
    : SRowIdx(std::min(_SRowIdx, _ERowIdx)),
      SColIdx(std::min(_SColIdx, _EColIdx)),
      SLayIdx(std::min(_SLayIdx, _ELayIdx)),
      ERowIdx(std::max(_SRowIdx, _ERowIdx)),
      EColIdx(std::max(_SColIdx, _EColIdx)),
      ELayIdx(std::max(_SLayIdx, _ELayIdx)), NetPtr(NetPtr) {}

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

void Route::reduceRouteSegments(std::vector<Route> &RouteSegments) {
  auto RouteSegmentCmp = [&](const Route &A, const Route &B) {
    auto TA = A.getType(), TB = B.getType();
    if (TA != TB)
      return TA < TB;
    switch (TA) {
    case Type::Horizontal: {
      if (A.SLayIdx != B.SLayIdx)
        return A.SLayIdx < B.SLayIdx;
      if (A.SRowIdx != B.SRowIdx)
        return A.SRowIdx < B.SRowIdx;
      return A.SColIdx < B.SColIdx;
    }
    case Type::Vertical: {
      if (A.SLayIdx != B.SLayIdx)
        return A.SLayIdx < B.SLayIdx;
      if (A.SColIdx != B.SColIdx)
        return A.SColIdx < B.SColIdx;
      return A.SRowIdx < B.SRowIdx;
    }
    case Type::Via: {
      if (A.SRowIdx != B.SRowIdx)
        return A.SRowIdx < B.SRowIdx;
      if (A.SColIdx != B.SColIdx)
        return A.SColIdx < B.SColIdx;
      return A.SLayIdx < B.SLayIdx;
    }
    default:
      assert(false && "Error RouteSegment::Type!");
    }
  };
  std::sort(RouteSegments.begin(), RouteSegments.end(), RouteSegmentCmp);
  std::vector<Route> NewRouteSegments;
  Route *CurRS = nullptr;
  for (auto &RS : RouteSegments) {
    bool NeedNewRS = false;
    auto Ty = RS.getType();
    if (CurRS == nullptr || Ty != CurRS->getType())
      NeedNewRS = true;
    else {
      bool RowCmp = CurRS->SRowIdx == RS.SRowIdx;
      bool ColCmp = CurRS->SColIdx == RS.SColIdx;
      bool ViaCmp = CurRS->SLayIdx == RS.SLayIdx;
      switch (Ty) {
      case Type::Horizontal: {
        if (RowCmp && ViaCmp && CurRS->EColIdx == RS.SColIdx)
          CurRS->EColIdx = RS.EColIdx;
        else
          NeedNewRS = true;
        break;
      }
      case Type::Vertical: {
        if (ColCmp && ViaCmp && CurRS->ERowIdx == RS.SRowIdx)
          CurRS->ERowIdx = RS.ERowIdx;
        else
          NeedNewRS = true;
        break;
      }
      case Type::Via: {
        if (RowCmp && ColCmp && CurRS->ELayIdx == RS.SLayIdx)
          CurRS->ELayIdx = RS.ELayIdx;
        else
          NeedNewRS = true;
        break;
      }
      default:
        NeedNewRS = true;
      }
    }
    if (NeedNewRS) {
      if (CurRS)
        NewRouteSegments.emplace_back(*CurRS);
      CurRS = &RS;
    }
  }
  if (CurRS) {
    NewRouteSegments.emplace_back(*CurRS);
  }
  RouteSegments = std::move(NewRouteSegments);
}

} // namespace Processed
} // namespace Input
} // namespace cell_move_router