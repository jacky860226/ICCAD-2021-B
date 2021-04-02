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
  class Pin {
  private:
    const Raw::Layer *PinLayer;

  public:
    Pin(const Raw::Layer *PinLayer);
    const Raw::Layer *getPinLayer() const { return PinLayer; }
  };
  class Blkg {
  private:
    const Raw::Layer *BlockageLayer;
    const int Demand;

  public:
    Blkg(const Raw::Layer *BlockageLayer, const int Demand);
    const Raw::Layer *getBlockageLayer() const { return BlockageLayer; }
    int getDemand() const { return Demand; }
  };

private:
  const Raw::MasterCell *RawMasterCell;
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
  const std::unordered_map<std::string, Pin> &getPinMap() const {
    return PinMap;
  }
  const std::unordered_map<std::string, Blkg> &getBlkgMap() const {
    return BlkgMap;
  }
  void to_ostream(std::ostream &out) const override;
};

class CellInst : Util::Outputable {
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
  class Pin {
    const CellInst *Inst;
    const MasterCell::Pin *MasterPin;

  public:
    Pin(const CellInst *Inst, const MasterCell::Pin *MasterPin);
    const CellInst *getInst() const { return Inst; }
    const MasterCell::Pin *getMasterPin() const { return MasterPin; }
  };

private:
  const Raw::Net *RawNet;
  const std::vector<Pin> Pins;
  const Raw::Layer *MinRoutingLayConstraint;

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