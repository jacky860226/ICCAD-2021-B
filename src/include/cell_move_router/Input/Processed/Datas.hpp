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

class CellInst : Util::Outputable {
  // const std::string InstName;
  // const std::string MasterCellName;
  // const int GGridRowIdx;
  // const int GGridColIdx;
  // const bool IsMovable;
  const Raw::CellInst *RawCellInst;
  const MasterCell *ProcessedMasterCell;

public:
  CellInst(const Raw::CellInst *RawCellInst,
           const std::unordered_map<std::string, MasterCell *> MasterCellMap);
  const std::string &getInstName() const { return RawCellInst->getInstName(); }
  const MasterCell *getMasterCell() const { return ProcessedMasterCell; }
  int getGGridRowIdx() const { return RawCellInst->getGGridRowIdx(); }
  int getGGridColIdx() const { return RawCellInst->getGGridColIdx(); }
  bool isMovable() const { return RawCellInst->isMovable(); }
  void to_ostream(std::ostream &out) const override;
};

class Net : Util::Outputable {
public:
  class Pin : Util::Outputable {
    // const std::string InstName;
    const CellInst *Inst;
    // const std::string MasterPinName;
    const MasterCell::Pin *MasterPin;

  public:
    // Pin(std::string &&InstName, std::string &&MasterPinName);
    Pin(const CellInst *Inst, const MasterCell::Pin *MasterPin);
    void to_ostream(std::ostream &out) const override;
    const CellInst *getInst() const { return Inst; }
    const MasterCell::Pin *getMasterPin() const { return MasterPin; }
  };

private:
  // const std::string NetName;
  const Raw::Net *RawNet;
  const std::vector<Pin> Pins;
  // const std::string MinRoutingLayConstraint;
  const Raw::Layer *MinRoutingLayConstraint;
  // const double Weight;

  static std::vector<Pin>
  CreatePins(const Raw::Net *RawNet,
             const std::unordered_map<std::string, CellInst *> CellInstMap);

public:
  Net(const Raw::Net *RawNet,
      const std::unordered_map<std::string, CellInst *> CellInstMap,
      const std::unordered_map<std::string, const Raw::Layer *> &LayerMap);
  const std::string &getNetName() const { return RawNet->getNetName(); }
  int getNumPins() const { return RawNet->getNumPins(); }
  const std::vector<Raw::Net::Pin> &getPins() const {
    return RawNet->getPins();
  }
  const Raw::Layer *getMinRoutingLayConstraint() const {
    return MinRoutingLayConstraint;
  }
  double getWeight() const { return RawNet->getWeight(); }
  void to_ostream(std::ostream &out) const override;
};

class VoltageArea : Util::Outputable {

private:
  const Raw::VoltageArea *RawVoltageArea;
  const std::vector<CellInst *> Instances;

  static std::vector<CellInst *> CreateInstances(
      const Raw::VoltageArea *RawVoltageArea,
      const std::unordered_map<std::string, CellInst *> CellInstMap);

public:
  VoltageArea(const Raw::VoltageArea *RawVoltageArea,
              const std::unordered_map<std::string, CellInst *> CellInstMap);
  void to_ostream(std::ostream &out) const override;
  const Raw::VoltageArea *const getRawVoltageArea() const {
    return RawVoltageArea;
  }
  const std::vector<CellInst *> &getInstances() const { return Instances; }
};

class Route : Util::Outputable {
  const int SRowIdx, SColIdx, SLayIdx;
  const int ERowIdx, EColIdx, ELayIdx;
  // const std::string NetName;
  const Net *NetPtr;

public:
  Route(Raw::Route *RawRoute, std::unordered_map<std::string, Net *> NetMap);
  void to_ostream(std::ostream &out) const override;
  int getSRowIdx() const { return SRowIdx; }
  int getSColIdx() const { return SColIdx; }
  int getSLayIdx() const { return SLayIdx; }
  int getERowIdx() const { return ERowIdx; }
  int getEColIdx() const { return EColIdx; }
  int getELayIdx() const { return ELayIdx; }
  const Net *getNetPtr() const { return NetPtr; }
};

} // namespace Processed
} // namespace Input
} // namespace cell_move_router