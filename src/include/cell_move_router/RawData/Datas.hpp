#pragma once
#include "Util/BaseType.hpp"
#include <ostream>
#include <string>
#include <vector>
namespace cell_move_router {
namespace RawData {
// TODO: implement the raw datas
class Layer : Util::Outputable {
  const std::string LayerName;
  const int Idx;
  const char RoutingDir;
  const int DefaultSupplyOfOneGGrid;
  const double PowerFactor;

public:
  Layer(std::string &&LayerName, const int Idx, const char RoutingDir,
        const int DefaultSupplyOfOneGGrid, const double PowerFactor);
  const std::string &getLayerName() const { return LayerName; }
  int getIdx() const { return Idx; }
  char getRoutingDir() const { return RoutingDir; }
  int getDefaultSupplyOfOneGGrid() const { return DefaultSupplyOfOneGGrid; }
  double getPowerFactor() const { return PowerFactor; }
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
  std::vector<Pin> Pins;
  std::vector<Blkg> Blkgs;

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
  void to_ostream(std::ostream &out) const override;
};
class VoltageArea : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class Route : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
} // namespace RawData
} // namespace cell_move_router