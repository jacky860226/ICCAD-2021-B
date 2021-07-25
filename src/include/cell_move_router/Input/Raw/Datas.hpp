#pragma once
#include "Util/BaseType.hpp"
#include <ostream>
#include <string>
#include <vector>
namespace cell_move_router {
namespace Input {
namespace Raw {
class Layer : Util::Outputable {
  std::string LayerName;
  int Idx;
  char RoutingDir;
  int DefaultSupplyOfOneGGrid;
  long long PowerFactor;

public:
  Layer(std::string &&LayerName, const int Idx, const char RoutingDir,
        const int DefaultSupplyOfOneGGrid, const long long PowerFactor);
  const std::string &getLayerName() const { return LayerName; }
  int getIdx() const { return Idx; }
  char getRoutingDir() const { return RoutingDir; }
  int getDefaultSupplyOfOneGGrid() const { return DefaultSupplyOfOneGGrid; }
  long long getPowerFactor() const { return PowerFactor; }
  void to_ostream(std::ostream &out) const override;
};
class NonDefaultSupplyGGrid : Util::Outputable {
  const int RowIdx, ColIdx, LayIdx;
  const int IncrOrDecrValue;

public:
  NonDefaultSupplyGGrid(const int RowIdx, const int ColIdx, const int LayIdx,
                        const int IncrOrDecrValue);
  int getRowIdx() const { return RowIdx; }
  int getColIdx() const { return ColIdx; }
  int getLayIdx() const { return LayIdx; }
  int getIncrOrDecrValue() const { return IncrOrDecrValue; }
  void to_ostream(std::ostream &out) const override;
};

class MasterCell : Util::Outputable {
public:
  class Pin : Util::Outputable {
  private:
    const std::string PinName;
    const std::string PinLayer;

  public:
    Pin(std::string &&PinName, std::string &&PinLayer);
    const std::string &getPinName() const { return PinName; }
    const std::string &getPinLayer() const { return PinLayer; }
    void to_ostream(std::ostream &out) const override;
  };
  class Blkg : Util::Outputable {
  private:
    const std::string BlockageName;
    const std::string BlockageLayer;
    const int Demand;

  public:
    Blkg(std::string &&BlockageName, std::string &&BlockageLayer,
         const int Demand);
    const std::string &getBlockageName() const { return BlockageName; }
    const std::string &getBlockageLayer() const { return BlockageLayer; }
    int getDemand() const { return Demand; }
    void to_ostream(std::ostream &out) const override;
  };

private:
  const std::string MasterCellName;
  const std::vector<Pin> Pins;
  const std::vector<Blkg> Blkgs;

public:
  MasterCell(std::string &&MasterCellName, std::vector<Pin> &&Pins,
             std::vector<Blkg> &&Blkgs);
  const std::string &getMasterCellName() const { return MasterCellName; }
  const std::vector<Pin> &getPins() const { return Pins; }
  const std::vector<Blkg> &getBlkgs() const { return Blkgs; }
  void to_ostream(std::ostream &out) const override;
};

class CellInst : Util::Outputable {
  const std::string InstName;
  const std::string MasterCellName;
  const int GGridRowIdx;
  const int GGridColIdx;
  const bool IsMovable;

public:
  CellInst(std::string &&InstName, std::string &&MasterCellName,
           const int GGridRowIdx, const int GGridColIdx, const bool IsMovable);
  const std::string &getInstName() const { return InstName; }
  const std::string &getMasterCellName() const { return MasterCellName; }
  int getGGridRowIdx() const { return GGridRowIdx; }
  int getGGridColIdx() const { return GGridColIdx; }
  bool isMovable() const { return IsMovable; }
  void to_ostream(std::ostream &out) const override;
};

class Net : Util::Outputable {
public:
  class Pin : Util::Outputable {
    const std::string InstName;
    const std::string MasterPinName;

  public:
    Pin(std::string &&InstName, std::string &&MasterPinName);
    const std::string &getInstName() const { return InstName; }
    const std::string &getMasterPinName() const { return MasterPinName; }
    void to_ostream(std::ostream &out) const override;
  };

private:
  const std::string NetName;
  const std::vector<Pin> Pins;
  const std::string MinRoutingLayConstraint;
  const long long Weight;

public:
  Net(std::string &&NetName, std::vector<Pin> &&Pins,
      std::string &&MinRoutingLayConstraint, const long long Weight);
  const std::string &getNetName() const { return NetName; }
  int getNumPins() const { return Pins.size(); }
  const std::vector<Pin> &getPins() const { return Pins; }
  const std::string &getMinRoutingLayConstraint() const {
    return MinRoutingLayConstraint;
  }
  long long getWeight() const { return Weight; }
  void to_ostream(std::ostream &out) const override;
};

class VoltageArea : Util::Outputable {
public:
  class GGrid : Util::Outputable {
  private:
    const int GGridRowIdx, GGridColIdx;

  public:
    GGrid(const int GGridRowIdx, const int GGridColIdx);
    int getGGridRowIdx() const { return GGridRowIdx; }
    int getGGridColIdx() const { return GGridColIdx; }
    void to_ostream(std::ostream &out) const override;
  };
  class Instance : Util::Outputable {
  private:
    const std::string InstanceName;

  public:
    Instance(std::string &&InstanceName);
    const std::string &getInstanceName() const { return InstanceName; }
    void to_ostream(std::ostream &out) const override;
  };

private:
  const std::string VoltageAreaName;
  const std::vector<GGrid> GGrids;
  const std::vector<Instance> Instances;

public:
  VoltageArea(std::string &&VoltageAreaName, std::vector<GGrid> &&GGrids,
              std::vector<Instance> &&Instances);
  void to_ostream(std::ostream &out) const override;
  const std::string &getVoltageAreaName() const { return VoltageAreaName; }
  const std::vector<GGrid> &getGGrids() const { return GGrids; }
  const std::vector<Instance> &getInstances() const { return Instances; }
};

class Route : Util::Outputable {
  const int SRowIdx, SColIdx, SLayIdx;
  const int ERowIdx, EColIdx, ELayIdx;
  const std::string NetName;

public:
  Route(const int SRowIdx, const int SColIdx, const int SLayIdx,
        const int ERowIdx, const int EColIdx, const int ELayIdx,
        std::string &&NetName);
  void to_ostream(std::ostream &out) const override;
  int getSRowIdx() const { return SRowIdx; }
  int getSColIdx() const { return SColIdx; }
  int getSLayIdx() const { return SLayIdx; }
  int getERowIdx() const { return ERowIdx; }
  int getEColIdx() const { return EColIdx; }
  int getELayIdx() const { return ELayIdx; }
  const std::string &getNetName() const { return NetName; }
};
} // namespace Raw
} // namespace Input
} // namespace cell_move_router