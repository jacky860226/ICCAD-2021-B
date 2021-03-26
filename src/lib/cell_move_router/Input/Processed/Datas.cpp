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

void MasterCell::Pin::to_ostream(std::ostream &out) const {
  // TODO
}

void MasterCell::Blkg::to_ostream(std::ostream &out) const {
  // TODO
}

void MasterCell::to_ostream(std::ostream &out) const {
  // TODO
}

MasterCell::Pin::Pin(const Raw::Layer *PinLayer) : PinLayer(PinLayer) {}

MasterCell::Blkg::Blkg(const Raw::Layer *BlockageLayer, const int Demand)
    : BlockageLayer(BlockageLayer), Demand(Demand) {}

std::unordered_map<std::string, MasterCell::Pin> MasterCell::CreatePinMap(
    const Raw::MasterCell *RawMasterCell,
    const std::unordered_map<std::string, const Raw::Layer *> &LayerMap) {
  std::unordered_map<std::string, MasterCell::Pin> PinMap;
  for (const auto &Pin : RawMasterCell->getPins()) {
    PinMap.emplace(Pin.getPinName(),
                   MasterCell::Pin(LayerMap.at(Pin.getPinLayer())));
  }
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
  // TODO
}

CellInst::CellInst(
    const Raw::CellInst *RawCellInst,
    const std::unordered_map<std::string, MasterCell *> MasterCellMap)
    : RawCellInst(RawCellInst),
      ProcessedMasterCell(
          MasterCellMap.at(RawCellInst->Raw::CellInst::getMasterCellName())) {}

// TODO
} // namespace Processed
} // namespace Input
} // namespace cell_move_router