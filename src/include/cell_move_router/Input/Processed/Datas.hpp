#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/Input/Raw/Datas.hpp"
#include <ostream>
#include <unordered_map>
namespace cell_move_router {
namespace Input {
namespace Processed {

class MasterCell : Util::Outputable {
public:
  class Pin : Util::Outputable {
  private:
    // const std::string PinName;
    // const std::string PinLayer;
    const Raw::Layer *PinLayer;

  public:
    Pin(const Raw::Layer *PinLayer);
    // const std::string &getPinName() const { return PinName; }
    const Raw::Layer *getPinLayer() const { return PinLayer; }
    void to_ostream(std::ostream &out) const override;
  };
  class Blkg : Util::Outputable {
  private:
    // const std::string BlockageName;
    // const std::string BlockageLayer;
    const Raw::Layer *BlockageLayer;
    const int Demand;

  public:
    Blkg(const Raw::Layer *BlockageLayer, const int Demand);
    // const std::string &getBlockageName() const { return BlockageName; }
    const Raw::Layer *getBlockageLayer() const { return BlockageLayer; }
    int getDemand() const { return Demand; }
    void to_ostream(std::ostream &out) const override;
  };

private:
  // const std::string MasterCellName;
  const Raw::MasterCell *RawMasterCell;
  // const std::vector<Pin> Pins;
  // const std::vector<Blkg> Blkgs;
  const std::unordered_map<std::string, Pin> PinMap;
  const std::unordered_map<std::string, Blkg> BlkgMap;

  static std::unordered_map<std::string, Pin> CreatePinMap(
      const Raw::MasterCell *RawMasterCell,
      const std::unordered_map<std::string, const Raw::Layer *> &LayerMap);
  static std::unordered_map<std::string, Blkg> CreateBlkgMap(
      const Raw::MasterCell *RawMasterCell,
      const std::unordered_map<std::string, const Raw::Layer *> &LayerMap);

public:
  MasterCell(
      const Raw::MasterCell *RawMasterCell,
      const std::unordered_map<std::string, const Raw::Layer *> &LayerMap);
  const std::string &getMasterCellName() const {
    return RawMasterCell->getMasterCellName();
  }
  // const std::vector<Pin> &getPins() const { return Pins; }
  // const std::vector<Blkg> &getBlkgs() const { return Blkgs; }
  const std::unordered_map<std::string, Pin> &getPinMap() const {
    return PinMap;
  }
  const std::unordered_map<std::string, Blkg> &getBlkgMap() const {
    return BlkgMap;
  }
  void to_ostream(std::ostream &out) const override;
};
// TODO
} // namespace Processed
} // namespace Input
} // namespace cell_move_router